// pass-fixture-runner — C++ end-to-end fixture runner for symbolon analysis
// passes. Compiles each .c fixture in --inputs to .bc, runs the pass binary
// against it, and diffs the resulting JSON against --golden/<name>.json.
//
// Replaces the three near-identical Python runner.py files previously living
// under each pass's tests/ directory.

#include <cstdint> // must precede cxxopts.hpp on GCC 15 (uint8_t)
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct Config {
  std::string tool;
  std::string binary;
  std::string inputs;
  std::string golden;
  std::string clang;
  std::string llvm_link;
  std::string filter;
  bool update_golden = false;

  // Derived from `tool` at the end of parseArgs. Avoids re-concatenating
  // these prefixes per fixture.
  std::string directive_args_prefix;
  std::string directive_nodbg_prefix;
  std::string directive_link_prefix;
};

bool parseArgs(int argc, char **argv, Config &cfg) {
  cxxopts::Options opts(
      "pass-fixture-runner",
      "End-to-end fixture runner for symbolon analysis passes");
  opts.add_options()("tool", "Tool name (used for directive prefix)",
                     cxxopts::value<std::string>())(
      "binary", "Path to the pass binary", cxxopts::value<std::string>())(
      "inputs", "Directory of .c fixtures", cxxopts::value<std::string>())(
      "golden", "Directory of .json goldens", cxxopts::value<std::string>())(
      "clang", "Clang executable",
      cxxopts::value<std::string>()->default_value("clang"))(
      "llvm-link",
      "llvm-link executable (used when fixtures use the link "
      "directive)",
      cxxopts::value<std::string>()->default_value("llvm-link"))(
      "filter", "Process only fixtures whose name contains this substring",
      cxxopts::value<std::string>()->default_value(""))(
      "update-golden",
      "Regenerate goldens instead of comparing")("h,help", "Show this message");

  auto r = opts.parse(argc, argv);
  if (r.count("help")) {
    std::cout << opts.help() << "\n";
    std::exit(0);
  }
  const char *required[] = {"tool", "binary", "inputs", "golden"};
  for (const char *f : required) {
    if (!r.count(f)) {
      std::cerr << "pass-fixture-runner: missing required --" << f << "\n";
      std::cerr << opts.help() << "\n";
      return false;
    }
  }
  cfg.tool = r["tool"].as<std::string>();
  cfg.binary = r["binary"].as<std::string>();
  cfg.inputs = r["inputs"].as<std::string>();
  cfg.golden = r["golden"].as<std::string>();
  cfg.clang = r["clang"].as<std::string>();
  cfg.llvm_link = r["llvm-link"].as<std::string>();
  cfg.filter = r["filter"].as<std::string>();
  cfg.update_golden = r.count("update-golden") > 0;
  cfg.directive_args_prefix = "// " + cfg.tool + ":";
  cfg.directive_nodbg_prefix = "// " + cfg.tool + "-no-debug";
  cfg.directive_link_prefix = "// " + cfg.tool + "-link:";
  return true;
}

std::optional<std::string> readFile(const std::string &path) {
  std::ifstream f(path);
  if (!f)
    return std::nullopt;
  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

bool writeFile(const std::string &path, const std::string &contents) {
  std::ofstream f(path);
  if (!f)
    return false;
  f << contents;
  return static_cast<bool>(f);
}

// Resolve an executable name or path to an absolute path via PATH lookup.
// If exe is already absolute, returns it unchanged.
std::string resolveExe(llvm::StringRef exe) {
  if (llvm::sys::path::is_absolute(exe))
    return exe.str();
  auto found = llvm::sys::findProgramByName(exe);
  if (found)
    return *found;
  return exe.str(); // fall back — ExecuteAndWait will report the error
}

// Run a command. Optionally redirect stdout/stderr to given file paths.
// Returns the exit code, or a negative value on internal launch failure.
int runCmd(llvm::StringRef exe, llvm::ArrayRef<llvm::StringRef> args,
           std::optional<llvm::StringRef> stdoutTo = std::nullopt,
           std::optional<llvm::StringRef> stderrTo = std::nullopt) {
  std::string abs_exe = resolveExe(exe);
  llvm::SmallVector<llvm::StringRef, 16> argv;
  argv.push_back(abs_exe);
  for (auto a : args)
    argv.push_back(a);

  std::optional<llvm::StringRef> redirects[3] = {std::nullopt, stdoutTo,
                                                 stderrTo};
  std::string err;
  int rc = llvm::sys::ExecuteAndWait(abs_exe, argv,
                                     /*Env=*/std::nullopt,
                                     /*Redirects=*/redirects,
                                     /*SecondsToWait=*/0,
                                     /*MemoryLimit=*/0, &err);
  if (rc < 0) {
    std::cerr << "pass-fixture-runner: failed to launch '" << exe.str()
              << "': " << err << "\n";
  }
  return rc;
}

std::string tempPath(const std::string &dir, llvm::StringRef name) {
  llvm::SmallString<128> p;
  p = dir;
  llvm::sys::path::append(p, name);
  return std::string(p);
}

struct Directives {
  std::vector<std::string> extra_args;
  std::vector<std::string> link_files;
  bool with_debug = true;
};

Directives parseDirectives(const std::string &src, const Config &cfg) {
  Directives d;
  std::istringstream stream(src);
  std::string line;
  while (std::getline(stream, line)) {
    size_t start = line.find_first_not_of(" \t");
    if (start == std::string::npos)
      continue;
    llvm::StringRef s(line.c_str() + start);
    if (s.starts_with(cfg.directive_link_prefix)) {
      std::string rest = s.substr(cfg.directive_link_prefix.size()).str();
      std::istringstream tok(rest);
      std::string w;
      while (tok >> w)
        d.link_files.push_back(w);
    } else if (s.starts_with(cfg.directive_nodbg_prefix)) {
      d.with_debug = false;
    } else if (s.starts_with(cfg.directive_args_prefix)) {
      std::string rest = s.substr(cfg.directive_args_prefix.size()).str();
      std::istringstream tok(rest);
      std::string w;
      while (tok >> w)
        d.extra_args.push_back(w);
    }
  }
  return d;
}

using json = nlohmann::json;

constexpr int kMaxDiffLines = 30;

void normalizePaths(json &actual) {
  if (actual.contains("module") && actual["module"].is_string()) {
    auto m = actual["module"].get<std::string>();
    actual["module"] = std::string(llvm::sys::path::filename(m));
  }
  if (actual.contains("files") && actual["files"].is_object()) {
    json new_files = json::object();
    for (auto &el : actual["files"].items()) {
      std::string base = std::string(llvm::sys::path::filename(el.key()));
      new_files[base] = el.value();
    }
    actual["files"] = new_files;
  }
}

bool diffJson(const std::string &name, const std::string &expected,
              const std::string &actual) {
  if (expected == actual)
    return true;
  std::cout << "FAIL: " << name << "\n";
  std::istringstream e(expected), a(actual);
  std::string el, al;
  bool e_more = static_cast<bool>(std::getline(e, el));
  bool a_more = static_cast<bool>(std::getline(a, al));
  int shown = 0;
  while (e_more || a_more) {
    if (e_more && a_more && el == al) {
      e_more = static_cast<bool>(std::getline(e, el));
      a_more = static_cast<bool>(std::getline(a, al));
      continue;
    }
    if (shown++ >= kMaxDiffLines) {
      std::cout << "  ... (truncated)\n";
      break;
    }
    if (e_more)
      std::cout << "  expected: " << el << "\n";
    else
      std::cout << "  expected: <EOF>\n";
    if (a_more)
      std::cout << "  actual:   " << al << "\n";
    else
      std::cout << "  actual:   <EOF>\n";
    if (e_more)
      e_more = static_cast<bool>(std::getline(e, el));
    if (a_more)
      a_more = static_cast<bool>(std::getline(a, al));
  }
  return false;
}

// Convenience wrapper around runCmd that accepts owned-string args
// (handles the StringRef-view construction internally).
int runCmdOwned(llvm::StringRef exe, const std::vector<std::string> &args,
                std::optional<llvm::StringRef> stdoutTo = std::nullopt,
                std::optional<llvm::StringRef> stderrTo = std::nullopt) {
  llvm::SmallVector<llvm::StringRef, 16> refs;
  refs.reserve(args.size());
  for (const auto &a : args)
    refs.push_back(a);
  return runCmd(exe, refs, stdoutTo, stderrTo);
}

// Compile a .c file to bitcode. Returns true on success; on failure writes
// the clang stderr to our stderr and returns false.
bool compileToBC(const Config &cfg, const fs::path &src_path,
                 const std::string &bc_out, bool with_debug,
                 const fs::path &temp_dir) {
  std::string clang_stderr = tempPath(
      temp_dir.string(), fs::path(bc_out).filename().string() + ".clang.err");
  std::vector<std::string> args{"-emit-llvm", "-O0",  "-c",
                                "-o",         bc_out, src_path.string()};
  if (with_debug)
    args.insert(args.begin() + 3, "-g");
  int rc =
      runCmdOwned(cfg.clang, args, std::nullopt, llvm::StringRef(clang_stderr));
  if (rc != 0) {
    std::cerr << "compile failed for " << src_path << ":\n";
    if (auto err = readFile(clang_stderr))
      std::cerr << *err;
    return false;
  }
  return true;
}

bool processFixture(const Config &cfg, const fs::path &src_path,
                    const fs::path &temp_dir) {
  std::string name = src_path.stem().string();

  auto src_text_opt = readFile(src_path.string());
  if (!src_text_opt) {
    std::cerr << "pass-fixture-runner: cannot read " << src_path << "\n";
    return false;
  }
  Directives d = parseDirectives(*src_text_opt, cfg);

  // 1. Compile fixture to .bc.
  std::string bc_path = tempPath(temp_dir.string(), name + ".bc");
  if (!compileToBC(cfg, src_path, bc_path, d.with_debug, temp_dir))
    return false;

  // 2. Optionally link additional .bc files.
  if (!d.link_files.empty()) {
    // Compile each link file to .bc first, then link them all together.
    std::vector<std::string> link_args{bc_path};
    for (size_t i = 0; i < d.link_files.size(); ++i) {
      auto &lf = d.link_files[i];
      // lf may be a bare filename — resolve relative to the inputs directory.
      fs::path lf_path = lf;
      if (lf_path.is_relative())
        lf_path = src_path.parent_path() / lf_path;
      std::string lf_bc =
          tempPath(temp_dir.string(), "link_" + std::to_string(i) + "_" +
                                          lf_path.stem().string() + ".bc");
      if (!compileToBC(cfg, lf_path, lf_bc, d.with_debug, temp_dir))
        return false;
      link_args.push_back(lf_bc);
    }
    link_args.push_back("-o");
    link_args.push_back(bc_path);
    if (runCmdOwned(cfg.llvm_link, link_args) != 0) {
      std::cerr << "llvm-link failed for " << src_path << "\n";
      return false;
    }
  }

  // 3. Run the pass binary.
  std::string tool_stdout = tempPath(temp_dir.string(), name + ".out");
  std::string tool_stderr = tempPath(temp_dir.string(), name + ".err");
  std::vector<std::string> tool_args;
  for (auto &a : d.extra_args)
    tool_args.push_back(a);
  tool_args.push_back(bc_path);
  int rc = runCmdOwned(cfg.binary, tool_args, llvm::StringRef(tool_stdout),
                       llvm::StringRef(tool_stderr));
  if (rc != 0) {
    std::cerr << cfg.tool << " failed for " << bc_path << ":\n";
    if (auto err = readFile(tool_stderr))
      std::cerr << *err;
    return false;
  }

  // 4. Parse and normalize the actual output.
  auto out_text = readFile(tool_stdout);
  if (!out_text) {
    std::cerr << "pass-fixture-runner: cannot read tool stdout\n";
    return false;
  }
  json actual;
  try {
    actual = json::parse(*out_text);
  } catch (const json::parse_error &e) {
    std::cerr << "pass-fixture-runner: JSON parse failed for " << name << ": "
              << e.what() << "\n";
    return false;
  }
  normalizePaths(actual);
  std::string actual_canon = actual.dump(2);

  fs::path golden_path = fs::path(cfg.golden) / (name + ".json");

  if (cfg.update_golden) {
    fs::create_directories(cfg.golden);
    if (!writeFile(golden_path.string(), actual_canon + "\n")) {
      std::cerr << "pass-fixture-runner: cannot write golden " << golden_path
                << "\n";
      return false;
    }
    std::cout << "updated " << golden_path << "\n";
    return true;
  }

  if (!fs::exists(golden_path)) {
    std::cout << "FAIL: " << name << ": missing golden " << golden_path << "\n";
    return false;
  }
  auto expected_text = readFile(golden_path.string());
  if (!expected_text) {
    std::cout << "FAIL: " << name << ": cannot read golden\n";
    return false;
  }
  json expected;
  try {
    expected = json::parse(*expected_text);
  } catch (const json::parse_error &e) {
    std::cout << "FAIL: " << name << ": golden JSON parse failed: " << e.what()
              << "\n";
    return false;
  }
  std::string expected_canon = expected.dump(2);
  if (!diffJson(name, expected_canon, actual_canon)) {
    return false;
  }
  std::cout << "PASS: " << name << "\n";
  return true;
}

struct TempDirGuard {
  fs::path dir;
  TempDirGuard(fs::path d) : dir(std::move(d)) {}
  ~TempDirGuard() {
    std::error_code ec;
    fs::remove_all(dir, ec); // no-throw cleanup
  }
  TempDirGuard(const TempDirGuard &) = delete;
  TempDirGuard &operator=(const TempDirGuard &) = delete;
};

} // namespace

int main(int argc, char **argv) {
  Config cfg;
  if (!parseArgs(argc, argv, cfg))
    return 1;

  std::vector<fs::path> inputs;
  for (auto &entry : fs::directory_iterator(cfg.inputs)) {
    if (entry.is_regular_file() && entry.path().extension() == ".c") {
      if (!cfg.filter.empty() && entry.path().filename().string().find(
                                     cfg.filter) == std::string::npos)
        continue;
      inputs.push_back(entry.path());
    }
  }
  std::sort(inputs.begin(), inputs.end());

  if (inputs.empty()) {
    std::cerr << "no fixtures found\n";
    return 1;
  }

  llvm::SmallString<128> temp_root;
  if (auto ec =
          llvm::sys::fs::createUniqueDirectory("pass-runner", temp_root)) {
    std::cerr << "pass-fixture-runner: cannot create temp dir: " << ec.message()
              << "\n";
    return 1;
  }
  TempDirGuard temp_dir_guard{fs::path(temp_root.c_str())};
  const fs::path &temp_dir = temp_dir_guard.dir;

  int failures = 0;
  for (const auto &src : inputs) {
    if (!processFixture(cfg, src, temp_dir))
      failures++;
  }

  return failures == 0 ? 0 : 1;
}

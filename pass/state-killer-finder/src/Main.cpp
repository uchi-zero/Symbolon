#include "StateKillerFinder.h"

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace llvm;

static cl::opt<std::string> InputFile(cl::Positional,
                                      cl::desc("<input.bc|input.ll>"));
static cl::opt<std::string>
    OutputFile("o", cl::desc("write JSON to <path> (default stdout)"),
               cl::value_desc("path"));
static cl::opt<std::string>
    EnableList("enable",
               cl::desc("comma-separated category names to enable "
                        "(see --list-categories for the catalog)"),
               cl::value_desc("cats"));
static cl::opt<bool>
    IncludeSystem("include-system",
                  cl::desc("do not skip findings whose source path looks "
                           "like a system header / KLEE runtime"));
static cl::opt<bool> Pretty("pretty",
                            cl::desc("pretty-print JSON with 2-space indent"));
static cl::opt<bool>
    ListCategories("list-categories",
                   cl::desc("print the category catalog (name / default / "
                            "severity) and exit"));

static void printVersion(raw_ostream &os) { os << "state-killer-finder 0.1\n"; }

static const std::vector<std::pair<statekillerfinder::Category, std::string>> &
allCategories() {
  using statekillerfinder::Category;
  static const auto v = []() {
    std::vector<std::pair<Category, std::string>> r;
    for (int i = 0; i < statekillerfinder::kCategoryCount; ++i) {
      Category c = static_cast<Category>(i);
      r.emplace_back(c, std::string(statekillerfinder::categoryName(c)));
    }
    return r;
  }();
  return v;
}

static std::set<statekillerfinder::Category> defaultEnabled() { return {}; }

static bool lookupCategory(const std::string &name,
                           statekillerfinder::Category &out) {
  for (auto &kv : allCategories()) {
    if (kv.second == name) {
      out = kv.first;
      return true;
    }
  }
  return false;
}

static int reportUnknownCategory(llvm::StringRef name) {
  llvm::errs() << "state-killer-finder: unknown category '" << name
               << "' (valid: ";
  bool first = true;
  for (auto &kv : allCategories()) {
    if (!first)
      llvm::errs() << ", ";
    llvm::errs() << kv.second;
    first = false;
  }
  llvm::errs() << ")\n";
  return 1;
}

static int resolveEnabled(std::set<statekillerfinder::Category> &enabled) {
  enabled = defaultEnabled();
  {
    llvm::SmallVector<llvm::StringRef, 8> parts;
    llvm::StringRef(EnableList).split(parts, ',', -1, /*KeepEmpty=*/false);
    for (llvm::StringRef raw : parts) {
      llvm::StringRef n = raw.trim();
      if (n.empty())
        continue;
      if (n == "all") {
        for (auto &kv : allCategories())
          enabled.insert(kv.first);
        continue;
      }
      std::string name(n);
      statekillerfinder::Category c;
      if (!lookupCategory(name, c))
        return reportUnknownCategory(n);
      enabled.insert(c);
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  cl::SetVersionPrinter(printVersion);
  cl::ParseCommandLineOptions(argc, argv, "state-killer-finder\n");

  if (ListCategories) {
    auto defaultSet = defaultEnabled();
    outs() << "Category                  Default  Severity\n";
    outs() << "------------------------  -------  -----------\n";
    for (auto &kv : allCategories()) {
      const char *def = defaultSet.count(kv.first) ? "on " : "off";
      auto sev = statekillerfinder::severityName(
          statekillerfinder::severityOf(kv.first));
      outs() << format("%-25s %s      %s\n", kv.second.c_str(), def,
                       std::string(sev).c_str());
    }
    return 0;
  }

  statekillerfinder::Options opts;
  if (int rc = resolveEnabled(opts.enabled); rc != 0)
    return rc;

  if (InputFile.empty()) {
    errs() << "state-killer-finder: no input file specified\n";
    return 1;
  }

  std::error_code ec;
  if (!std::filesystem::exists(InputFile.getValue(), ec) || ec) {
    errs() << "state-killer-finder: cannot open " << InputFile.getValue()
           << ": file not found\n";
    return 1;
  }

  LLVMContext ctx;
  SMDiagnostic err;
  std::unique_ptr<Module> mod = parseIRFile(InputFile, err, ctx);
  if (!mod) {
    err.print("state-killer-finder", errs());
    return 2;
  }
  opts.include_system = IncludeSystem;
  opts.pretty = Pretty;

  statekillerfinder::StateKillerFinderPass pass(opts);

  PassBuilder pb;
  ModuleAnalysisManager mam;
  pb.registerModuleAnalyses(mam);
  pass.run(*mod, mam);

  auto json =
      statekillerfinder::renderReport(pass.findings(), opts, mod->getName());
  std::string text = opts.pretty ? json.dump(2) : json.dump();

  if (OutputFile.empty()) {
    outs() << text << "\n";
  } else {
    std::ofstream f(OutputFile);
    if (!f) {
      errs() << "state-killer-finder: cannot open " << OutputFile << "\n";
      return 1;
    }
    f << text << "\n";
  }
  return 0;
}

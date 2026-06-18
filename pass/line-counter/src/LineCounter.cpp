#include "LineCounter.h"

#include <symbolon/Paths.h>

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>

namespace linecounter {

LineMap collectLines(const llvm::Module &M, const Options &opts) {
  LineMap out;
  for (const llvm::Function &F : M) {
    for (const llvm::BasicBlock &BB : F) {
      for (const llvm::Instruction &I : BB) {
        const llvm::DebugLoc &dl = I.getDebugLoc();
        if (!dl)
          continue;
        unsigned line = dl.getLine();
        if (line == 0)
          continue;
        std::string file =
            symbolon::joinSourcePath(dl->getDirectory(), dl->getFilename());
        if (file.empty())
          continue;
        if (!opts.include_system && symbolon::looksLikeSystemPath(file))
          continue;
        out[file].insert(line);
      }
    }
  }
  return out;
}

nlohmann::ordered_json renderReport(const LineMap &lines,
                                    std::string_view moduleName) {
  using json = nlohmann::ordered_json;

  std::size_t total_lines = 0;
  for (const auto &kv : lines)
    total_lines += kv.second.size();

  json out = json::object();
  out["schema_version"] = 1;
  out["module"] = std::string(moduleName);

  json summary = json::object();
  summary["total_files"] = lines.size();
  summary["total_lines"] = total_lines;
  out["summary"] = summary;

  json files = json::object();
  for (const auto &kv : lines) {
    json entry = json::object();
    entry["line_count"] = kv.second.size();
    json arr = json::array();
    for (unsigned l : kv.second) // std::set iterates ascending
      arr.push_back(l);
    entry["lines"] = arr;
    files[kv.first] = entry;
  }
  out["files"] = files;
  return out;
}

} // namespace linecounter

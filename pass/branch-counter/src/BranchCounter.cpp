#include "BranchCounter.h"

#include <symbolon/Paths.h>

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>

#include <algorithm>

namespace branchcounter {

FileBranches collectBranches(const llvm::Module &M, const Options &opts) {
  FileBranches out;
  for (const llvm::Function &F : M) {
    for (const llvm::BasicBlock &BB : F) {
      const llvm::Instruction *term = BB.getTerminator();
      if (!term)
        continue;
      const llvm::DebugLoc &dl = term->getDebugLoc();
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

      Branch b;
      b.line = line;
      if (const auto *br = llvm::dyn_cast<llvm::BranchInst>(term)) {
        if (!br->isConditional())
          continue;
        b.opcode = "br";
        b.outcomes = 2;
      } else if (const auto *sw = llvm::dyn_cast<llvm::SwitchInst>(term)) {
        b.opcode = "switch";
        b.outcomes = sw->getNumCases() + 1; // cases + default
      } else {
        continue;
      }
      out[file].push_back(std::move(b));
    }
  }
  // Sort each file's branches by (line, opcode) for stable golden output.
  for (auto &kv : out) {
    std::sort(kv.second.begin(), kv.second.end(),
              [](const Branch &a, const Branch &b) {
                if (a.line != b.line)
                  return a.line < b.line;
                return a.opcode < b.opcode;
              });
  }
  return out;
}

nlohmann::ordered_json renderReport(const FileBranches &branches,
                                    std::string_view moduleName) {
  using json = nlohmann::ordered_json;

  std::size_t total_branches = 0;
  std::size_t total_outcomes = 0;
  for (const auto &kv : branches) {
    total_branches += kv.second.size();
    for (const auto &b : kv.second)
      total_outcomes += b.outcomes;
  }

  json out = json::object();
  out["schema_version"] = 1;
  out["module"] = std::string(moduleName);

  json summary = json::object();
  summary["total_files"] = branches.size();
  summary["total_branches"] = total_branches;
  summary["total_outcomes"] = total_outcomes;
  out["summary"] = summary;

  json files = json::object();
  for (const auto &kv : branches) {
    std::size_t file_outcomes = 0;
    for (const auto &b : kv.second)
      file_outcomes += b.outcomes;

    json entry = json::object();
    entry["branch_count"] = kv.second.size();
    entry["outcome_count"] = file_outcomes;
    json arr = json::array();
    for (const auto &b : kv.second) {
      json item = json::object();
      item["line"] = b.line;
      item["opcode"] = b.opcode;
      item["outcomes"] = b.outcomes;
      arr.push_back(item);
    }
    entry["branches"] = arr;
    files[kv.first] = entry;
  }
  out["files"] = files;
  return out;
}

} // namespace branchcounter

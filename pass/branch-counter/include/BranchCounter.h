#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>

namespace branchcounter {

struct Options {
  bool include_system = false;
  bool pretty = false;
};

struct Branch {
  unsigned line = 0;
  std::string opcode; // "br" or "switch"
  unsigned outcomes = 0;
};

// file -> branches in that file (insertion order = sorted by (line, opcode)).
using FileBranches = std::map<std::string, std::vector<Branch>>;

// Walks every basic-block terminator in `M`. For each conditional BranchInst
// (2 outcomes) and SwitchInst (N cases + default = N+1 outcomes), records a
// Branch at its source DebugLoc. Skips terminators without debug info or
// with line == 0, and (unless opts.include_system) skips files matching
// symbolon::looksLikeSystemPath.
FileBranches collectBranches(const llvm::Module &M, const Options &opts);

// Renders the FileBranches to the schema in the spec.
nlohmann::ordered_json renderReport(const FileBranches &branches,
                                    std::string_view moduleName);

} // namespace branchcounter

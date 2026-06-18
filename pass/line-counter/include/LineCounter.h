#pragma once

#include <map>
#include <set>
#include <string>
#include <string_view>

#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>

namespace linecounter {

struct Options {
  bool include_system = false;
  bool pretty = false;
};

// file -> set of source lines present in the IR.
using LineMap = std::map<std::string, std::set<unsigned>>;

// Walks every Instruction in `M` and inserts (file, line) pairs from
// non-null debug locations whose line != 0. Filters paths matching
// looksLikeSystemPath() unless opts.include_system is true.
LineMap collectLines(const llvm::Module &M, const Options &opts);

// Renders the LineMap to the schema described in the spec. Uses
// ordered_json so file keys keep their alphabetical order and `lines`
// arrays stay sorted ascending.
nlohmann::ordered_json renderReport(const LineMap &lines,
                                    std::string_view moduleName);

} // namespace linecounter

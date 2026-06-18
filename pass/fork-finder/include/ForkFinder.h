#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/PassManager.h>
#include <nlohmann/json.hpp>

namespace forkfinder {

enum class Category : std::uint8_t {
  Conditional,
  Switch,
  Indirect,
  Call,
  MemOp,
  Alloc,
  Realloc,
  Free,
  DivByZero,
  OverflowCheck,
  LibraryLoop,
};

std::string_view categoryName(Category c);
bool isLibraryLoopFunction(llvm::StringRef name);

struct Finding {
  std::string file; // empty => unlocated
  std::string function;
  unsigned line = 0;
  std::string opcode; // LLVM opcode name, e.g. "br", "udiv", "load"
  Category category = Category::Conditional;
  unsigned loop_depth = 0;
};

struct Options {
  bool include_implicit = false;
  bool include_system = false;
  bool include_non_loop = false;
  bool pretty = false;
};

// NOTE: returns nlohmann::ordered_json (not nlohmann::json) so that the
// line-number keys serialize in numeric ascending order. nlohmann::json
// uses std::map<std::string,...> internally and would lex-sort "100"
// before "9".
nlohmann::ordered_json renderReport(const std::vector<Finding> &findings,
                                    const Options &opts,
                                    std::string_view moduleName);

class ForkFinderPass : public llvm::PassInfoMixin<ForkFinderPass> {
public:
  explicit ForkFinderPass(Options opts) : opts_(opts) {}
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  const std::vector<Finding> &findings() const { return findings_; }
  static llvm::StringRef name() { return "ForkFinderPass"; }

private:
  Options opts_;
  std::vector<Finding> findings_;
};

} // namespace forkfinder

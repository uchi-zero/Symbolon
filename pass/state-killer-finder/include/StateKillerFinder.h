#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/PassManager.h>
#include <nlohmann/json.hpp>

namespace statekillerfinder {

enum class Severity : std::uint8_t { Definite, Likely, Conditional };

enum class Category : std::uint8_t {
  ProgramTerminator,
  UnreachableInstruction,
  UnsupportedInstruction,
  UnimplementedIntrinsic,
  ExternCallSymArg,
  ExternCallSymPtr,
  ExternCallIndirect,
  InlineAsm,
  IndirectBranch,
  SymbolicAllocSize,
  SymbolicVectorIndex,
  CallArityMismatch,
  FreeOfNonHeap,
};

inline constexpr int kCategoryCount = 13;

std::string_view categoryName(Category c);
std::string_view severityName(Severity s);
Severity severityOf(Category c);

bool isProgramTerminator(llvm::StringRef name);
bool isKleeHandledIntrinsic(llvm::StringRef stripped_name);

struct Finding {
  std::string file; // empty => unlocated
  std::string function;
  unsigned line = 0;
  std::string opcode; // "call" or "invoke"
  std::string callee; // external function name, or "<indirect>"
  Category category = Category::ProgramTerminator;
};

struct Options {
  std::set<Category> enabled; // resolved set after CLI parsing
  bool include_system = false;
  bool pretty = false;
};

// NOTE: returns nlohmann::ordered_json (not nlohmann::json) so that the
// line-number keys serialize in numeric ascending order. nlohmann::json
// uses std::map<std::string,...> internally and would lex-sort "100"
// before "9".
nlohmann::ordered_json renderReport(const std::vector<Finding> &findings,
                                    const Options &opts,
                                    std::string_view moduleName);

class StateKillerFinderPass
    : public llvm::PassInfoMixin<StateKillerFinderPass> {
public:
  explicit StateKillerFinderPass(Options opts) : opts_(std::move(opts)) {}
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  const std::vector<Finding> &findings() const { return findings_; }
  static llvm::StringRef name() { return "StateKillerFinderPass"; }

private:
  Options opts_;
  std::vector<Finding> findings_;
};

} // namespace statekillerfinder

#include "StateKillerFinder.h"
#include <llvm/IR/Module.h>

namespace statekillerfinder {
void runVisitor(llvm::Module &M, std::vector<Finding> &out,
                const Options &opts);

llvm::PreservedAnalyses
StateKillerFinderPass::run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
  findings_.clear();
  runVisitor(M, findings_, opts_);
  return llvm::PreservedAnalyses::all();
}
} // namespace statekillerfinder

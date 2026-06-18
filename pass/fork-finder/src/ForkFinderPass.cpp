#include "Internal.h"

namespace forkfinder {

llvm::PreservedAnalyses ForkFinderPass::run(llvm::Module &M,
                                            llvm::ModuleAnalysisManager &) {
  findings_.clear();
  runVisitor(M, findings_, opts_);
  return llvm::PreservedAnalyses::all();
}

} // namespace forkfinder

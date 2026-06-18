#pragma once

#include "ForkFinder.h"

#include <llvm/IR/Module.h>

#include <vector>

namespace forkfinder {

void runVisitor(llvm::Module &M, std::vector<Finding> &out,
                const Options &opts);

} // namespace forkfinder

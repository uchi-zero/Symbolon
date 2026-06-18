#include "Internal.h"

#include <symbolon/Paths.h>

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstVisitor.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Module.h>

namespace forkfinder {
namespace {

class ForkFinderVisitor : public llvm::InstVisitor<ForkFinderVisitor> {
public:
  ForkFinderVisitor(std::vector<Finding> &out, const Options &opts,
                    const llvm::LoopInfo &LI)
      : out_(out), opts_(opts), LI_(LI) {}

  void visitBranchInst(llvm::BranchInst &I) {
    if (I.isUnconditional())
      return;
    emit(I, Category::Conditional, "br");
  }

  void visitSwitchInst(llvm::SwitchInst &I) {
    emit(I, Category::Switch, "switch");
  }

  void visitIndirectBrInst(llvm::IndirectBrInst &I) {
    emit(I, Category::Indirect, "indirectbr");
  }

  void visitLoadInst(llvm::LoadInst &I) { emit(I, Category::MemOp, "load"); }
  void visitStoreInst(llvm::StoreInst &I) { emit(I, Category::MemOp, "store"); }
  void visitAtomicRMWInst(llvm::AtomicRMWInst &I) {
    emit(I, Category::MemOp, "atomicrmw");
  }
  void visitAtomicCmpXchgInst(llvm::AtomicCmpXchgInst &I) {
    emit(I, Category::MemOp, "cmpxchg");
  }

  void visitAllocaInst(llvm::AllocaInst &I) {
    // Skip "static" allocas where the array size is a known constant
    // (including the implicit 1 for plain `alloca i32`).
    llvm::Value *size = I.getArraySize();
    if (auto *ci = llvm::dyn_cast<llvm::ConstantInt>(size)) {
      (void)ci;
      return;
    }
    emit(I, Category::Alloc, "alloca");
  }

  void visitCallBase(llvm::CallBase &I) {
    // Inline asm is not a KLEE fork site at the IR level.
    if (I.isInlineAsm())
      return;
    llvm::Function *callee = I.getCalledFunction();
    // Indirect call: callee not a constant Function*.
    if (callee == nullptr) {
      emit(I, Category::Call,
           llvm::isa<llvm::InvokeInst>(I) ? "invoke" : "call");
      return;
    }
    // LibraryLoop — named libc byte-loop functions OR
    // llvm.memcpy/memset/memmove intrinsics. Emitted regardless of source-level
    // loop depth.
    llvm::Intrinsic::ID iid = callee->getIntrinsicID();
    bool is_library_loop =
        iid == llvm::Intrinsic::memcpy || iid == llvm::Intrinsic::memset ||
        iid == llvm::Intrinsic::memmove ||
        (!callee->isIntrinsic() && isLibraryLoopFunction(callee->getName()));
    if (is_library_loop) {
      emit(I, Category::LibraryLoop, "call");
      return;
    }

    llvm::StringRef name = callee->getName();

    // Implicit overflow-check intrinsics (only with --include-implicit).
    if (callee->isIntrinsic() && opts_.include_implicit) {
      if (name.startswith("llvm.sadd.with.overflow.") ||
          name.startswith("llvm.uadd.with.overflow.") ||
          name.startswith("llvm.ssub.with.overflow.") ||
          name.startswith("llvm.usub.with.overflow.") ||
          name.startswith("llvm.smul.with.overflow.") ||
          name.startswith("llvm.umul.with.overflow.")) {
        emit(I, Category::OverflowCheck, "call");
        return;
      }
    }
    // Skip all other LLVM intrinsics — they don't fork in KLEE for our
    // purposes.
    if (callee->isIntrinsic())
      return;

    // Allocator name -> index of the size operand (or kCallocPair for
    // calloc(nmemb, size), which checks BOTH operands 0 and 1).
    static constexpr int kCallocPair = -1;
    struct AllocSpec {
      llvm::StringRef name;
      int size_idx;
    };
    static const AllocSpec kAllocators[] = {
        {"malloc", 0},           {"valloc", 0},   {"pvalloc", 0},
        {"aligned_alloc", 1},    {"memalign", 1}, {"posix_memalign", 2},
        {"calloc", kCallocPair},
    };
    for (const AllocSpec &a : kAllocators) {
      if (name != a.name)
        continue;
      bool sym = false;
      if (a.size_idx == kCallocPair) {
        if (I.arg_size() >= 2) {
          sym = !llvm::isa<llvm::ConstantInt>(I.getArgOperand(0)) ||
                !llvm::isa<llvm::ConstantInt>(I.getArgOperand(1));
        }
      } else {
        unsigned idx = static_cast<unsigned>(a.size_idx);
        if (I.arg_size() > idx) {
          sym = !llvm::isa<llvm::ConstantInt>(I.getArgOperand(idx));
        }
      }
      if (sym)
        emit(I, Category::Alloc, "call");
      return;
    }

    if (name == "realloc") {
      if (I.arg_size() >= 2 &&
          !llvm::isa<llvm::ConstantInt>(I.getArgOperand(1))) {
        emit(I, Category::Realloc, "call");
      }
      return;
    }

    if (name == "free") {
      emit(I, Category::Free, "call");
      return;
    }

    // Other direct calls: not a fork site at the call instruction itself.
    return;
  }

  void visitBinaryOperator(llvm::BinaryOperator &I) {
    if (!opts_.include_implicit)
      return;
    unsigned op = I.getOpcode();
    if (op != llvm::Instruction::UDiv && op != llvm::Instruction::SDiv &&
        op != llvm::Instruction::URem && op != llvm::Instruction::SRem)
      return;
    llvm::Value *divisor = I.getOperand(1);
    if (auto *ci = llvm::dyn_cast<llvm::ConstantInt>(divisor)) {
      if (!ci->isZero())
        return; // safe: const non-zero divisor never forks
    }
    const char *opname = "udiv";
    switch (op) {
    case llvm::Instruction::UDiv:
      opname = "udiv";
      break;
    case llvm::Instruction::SDiv:
      opname = "sdiv";
      break;
    case llvm::Instruction::URem:
      opname = "urem";
      break;
    case llvm::Instruction::SRem:
      opname = "srem";
      break;
    }
    emit(I, Category::DivByZero, opname);
  }

private:
  void emit(llvm::Instruction &I, Category c, const char *opcode) {
    unsigned depth = LI_.getLoopDepth(I.getParent());
    if (c != Category::LibraryLoop && depth == 0 && !opts_.include_non_loop)
      return;
    Finding f;
    f.category = c;
    f.opcode = opcode;
    f.loop_depth = depth;
    if (auto *fn = I.getFunction())
      f.function = fn->getName().str();
    if (const llvm::DebugLoc &dl = I.getDebugLoc()) {
      f.file = symbolon::joinSourcePath(dl->getDirectory(), dl->getFilename());
      f.line = dl.getLine();
    }
    out_.push_back(std::move(f));
  }

  std::vector<Finding> &out_;
  const Options &opts_;
  const llvm::LoopInfo &LI_;
};

} // namespace

void runVisitor(llvm::Module &M, std::vector<Finding> &out,
                const Options &opts) {
  for (llvm::Function &F : M) {
    if (F.isDeclaration())
      continue;
    llvm::DominatorTree DT(F);
    llvm::LoopInfo LI(DT);
    ForkFinderVisitor v(out, opts, LI);
    v.visit(F);
  }
}

} // namespace forkfinder

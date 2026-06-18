#include "StateKillerFinder.h"

#include <symbolon/Paths.h>

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/InstVisitor.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Module.h>

namespace statekillerfinder {
namespace {

static llvm::Function *resolveCallTarget(const llvm::CallBase &I) {
  if (llvm::Function *F = I.getCalledFunction())
    return F;
  llvm::Value *op = I.getCalledOperand();
  if (op == nullptr)
    return nullptr;
  return llvm::dyn_cast<llvm::Function>(op->stripPointerCasts());
}

static bool isSymbolicHeapAlloc(llvm::StringRef name, const llvm::CallBase &I) {
  // Returns true if the callee is a heap allocator and at least one
  // size-shaped operand is not a Constant.
  if (name == "malloc" || name == "memalign" || name == "aligned_alloc") {
    if (name == "malloc") {
      return I.arg_size() >= 1 &&
             !llvm::isa<llvm::Constant>(I.getArgOperand(0));
    }
    return I.arg_size() >= 2 && !llvm::isa<llvm::Constant>(I.getArgOperand(1));
  }
  if (name == "calloc") {
    return I.arg_size() >= 2 &&
           (!llvm::isa<llvm::Constant>(I.getArgOperand(0)) ||
            !llvm::isa<llvm::Constant>(I.getArgOperand(1)));
  }
  if (name == "realloc") {
    return I.arg_size() >= 2 && !llvm::isa<llvm::Constant>(I.getArgOperand(1));
  }
  return false;
}

class Visitor : public llvm::InstVisitor<Visitor> {
public:
  Visitor(std::vector<Finding> &out, const Options &opts)
      : out_(out), opts_(opts) {}

  void visitCallBase(llvm::CallBase &I) {
    const char *opcode = llvm::isa<llvm::InvokeInst>(I) ? "invoke" : "call";
    if (I.isInlineAsm()) {
      if (opts_.enabled.count(Category::InlineAsm))
        emit(I, opcode, "<inline-asm>", Category::InlineAsm);
      return;
    }

    // CallArityMismatch: direct or bitcast-resolvable indirect call where
    // the callsite passes fewer args than the callee declares. Placed
    // before the indirect-call short-circuit because the example shape
    // (function-pointer cast) has getCalledFunction() == nullptr.
    if (llvm::Function *resolved = resolveCallTarget(I);
        resolved != nullptr && !resolved->isVarArg() &&
        I.arg_size() < resolved->getFunctionType()->getNumParams()) {
      if (opts_.enabled.count(Category::CallArityMismatch))
        emit(I, opcode, resolved->getName(), Category::CallArityMismatch);
      // Fall through: the call may also classify as ExternCallIndirect /
      // ExternCallSym* below; the JSON dedup-per-line handles overlap.
    }

    llvm::Function *callee = I.getCalledFunction();
    if (callee == nullptr) {
      if (opts_.enabled.count(Category::ExternCallIndirect))
        emit(I, opcode, "<indirect>", Category::ExternCallIndirect);
      return;
    }

    // ProgramTerminator: callee name (or trap intrinsic name) is in
    // the program-terminator allowlist.
    if (isProgramTerminator(callee->getName())) {
      if (opts_.enabled.count(Category::ProgramTerminator))
        emit(I, opcode, callee->getName(), Category::ProgramTerminator);
      return; // No further per-arg analysis for this site.
    }

    if (callee->isIntrinsic()) {
      llvm::StringRef stripped =
          llvm::Intrinsic::getName(callee->getIntrinsicID());
      if (!isKleeHandledIntrinsic(stripped)) {
        if (opts_.enabled.count(Category::UnimplementedIntrinsic))
          emit(I, opcode, callee->getName(), Category::UnimplementedIntrinsic);
      }
      return;
    }
    // SymbolicAllocSize: heap allocator with a non-constant size operand.
    if (isSymbolicHeapAlloc(callee->getName(), I)) {
      if (opts_.enabled.count(Category::SymbolicAllocSize))
        emit(I, opcode, callee->getName(), Category::SymbolicAllocSize);
      return; // takes precedence over the generic per-arg ExternCallSym* loop
    }
    // FreeOfNonHeap: free() where the arg traces directly to an alloca
    // or a global.
    if (callee->getName() == "free" && I.arg_size() >= 1) {
      llvm::Value *p = I.getArgOperand(0)->stripPointerCasts();
      if (llvm::isa<llvm::AllocaInst>(p) ||
          llvm::isa<llvm::GlobalVariable>(p)) {
        if (opts_.enabled.count(Category::FreeOfNonHeap))
          emit(I, opcode, "free", Category::FreeOfNonHeap);
        return;
      }
    }
    if (!callee->isDeclaration())
      return; // body present => KLEE executes symbolically, not external.

    for (unsigned i = 0; i < I.arg_size(); ++i) {
      llvm::Value *arg = I.getArgOperand(i);
      if (llvm::isa<llvm::Constant>(arg))
        continue;
      llvm::Type *T = arg->getType();
      if (T->isPointerTy()) {
        if (!opts_.enabled.count(Category::ExternCallSymPtr))
          continue;
        emit(I, opcode, callee->getName(), Category::ExternCallSymPtr);
        continue;
      }
      if (!opts_.enabled.count(Category::ExternCallSymArg))
        continue;
      emit(I, opcode, callee->getName(), Category::ExternCallSymArg);
    }
  }

  void visitUnreachableInst(llvm::UnreachableInst &I) {
    if (opts_.enabled.count(Category::UnreachableInstruction))
      emit(I, "unreachable", "", Category::UnreachableInstruction);
  }

  void visitIndirectBrInst(llvm::IndirectBrInst &I) {
    if (opts_.enabled.count(Category::IndirectBranch))
      emit(I, "indirectbr", "", Category::IndirectBranch);
  }

  void visitVAArgInst(llvm::VAArgInst &I) { emitUnsupported(I, "va_arg"); }
  void visitAtomicRMWInst(llvm::AtomicRMWInst &I) {
    emitUnsupported(I, "atomicrmw");
  }
  void visitAtomicCmpXchgInst(llvm::AtomicCmpXchgInst &I) {
    emitUnsupported(I, "cmpxchg");
  }
  void visitResumeInst(llvm::ResumeInst &I) { emitUnsupported(I, "resume"); }
  void visitLandingPadInst(llvm::LandingPadInst &I) {
    emitUnsupported(I, "landingpad");
  }
  void visitShuffleVectorInst(llvm::ShuffleVectorInst &I) {
    emitUnsupported(I, "shufflevector");
  }

  void visitAllocaInst(llvm::AllocaInst &I) {
    if (!I.isArrayAllocation())
      return;
    if (llvm::isa<llvm::Constant>(I.getArraySize()))
      return;
    if (opts_.enabled.count(Category::SymbolicAllocSize))
      emit(I, "alloca", "", Category::SymbolicAllocSize);
  }

  void visitExtractElementInst(llvm::ExtractElementInst &I) {
    if (llvm::isa<llvm::Constant>(I.getIndexOperand()))
      return;
    if (opts_.enabled.count(Category::SymbolicVectorIndex))
      emit(I, "extractelement", "", Category::SymbolicVectorIndex);
  }
  void visitInsertElementInst(llvm::InsertElementInst &I) {
    // Operand 2 is the index for insertelement.
    if (llvm::isa<llvm::Constant>(I.getOperand(2)))
      return;
    if (opts_.enabled.count(Category::SymbolicVectorIndex))
      emit(I, "insertelement", "", Category::SymbolicVectorIndex);
  }

private:
  void emitUnsupported(llvm::Instruction &I, const char *opcode) {
    if (opts_.enabled.count(Category::UnsupportedInstruction))
      emit(I, opcode, "", Category::UnsupportedInstruction);
  }

  void emit(llvm::Instruction &I, const char *opcode, llvm::StringRef callee,
            Category c) {
    Finding f;
    f.opcode = opcode;
    f.category = c;
    f.callee = callee.str();
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
};

} // namespace

void runVisitor(llvm::Module &M, std::vector<Finding> &out,
                const Options &opts) {
  Visitor v(out, opts);
  v.visit(M);
}

} // namespace statekillerfinder

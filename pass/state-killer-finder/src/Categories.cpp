#include "StateKillerFinder.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/StringSet.h>

#include <array>

namespace statekillerfinder {
namespace {

constexpr std::array<llvm::StringLiteral, 13> kProgramTerminators{
    // Named functions (SpecialFunctionHandler DNR list).
    "abort",
    "exit",
    "_exit",
    "_Exit",
    "__assert",
    "__assert_fail",
    "__assert_rtn",
    "_assert",
    "klee_abort",
    "klee_silent_exit",
    "klee_report_error",
    // Trap intrinsics (KLEE handler terminates state).
    "llvm.trap",
    "llvm.debugtrap",
};

constexpr std::array<llvm::StringLiteral, 42> kKleeHandledIntrinsics{
    // Memory intrinsics.
    "llvm.memcpy",
    "llvm.memmove",
    "llvm.memset",
    // Floating-point intrinsics.
    "llvm.fabs",
    "llvm.fma",
    "llvm.fmuladd",
    // Integer arithmetic intrinsics.
    "llvm.abs",
    "llvm.smax",
    "llvm.smin",
    "llvm.umax",
    "llvm.umin",
    "llvm.fshl",
    "llvm.fshr",
    "llvm.ctlz",
    "llvm.cttz",
    "llvm.ctpop",
    "llvm.bswap",
    // Overflow-check intrinsics.
    "llvm.sadd.with.overflow",
    "llvm.uadd.with.overflow",
    "llvm.ssub.with.overflow",
    "llvm.usub.with.overflow",
    "llvm.smul.with.overflow",
    "llvm.umul.with.overflow",
    // Saturating arithmetic intrinsics.
    "llvm.sadd.sat",
    "llvm.uadd.sat",
    "llvm.ssub.sat",
    "llvm.usub.sat",
    // Debug intrinsics (no-op in KLEE).
    "llvm.dbg.declare",
    "llvm.dbg.value",
    "llvm.dbg.addr",
    "llvm.dbg.label",
    // Lifetime intrinsics (no-op in KLEE).
    "llvm.lifetime.start",
    "llvm.lifetime.end",
    // Varargs intrinsics (lowered or handled by KLEE).
    "llvm.va_start",
    "llvm.va_end",
    "llvm.va_copy",
    // Stack-save/restore intrinsics.
    "llvm.stacksave",
    "llvm.stackrestore",
    // Misc.
    "llvm.objectsize",
    "llvm.assume",
    "llvm.expect",
    "llvm.prefetch",
};

} // namespace

bool isProgramTerminator(llvm::StringRef name) {
  for (auto t : kProgramTerminators)
    if (name == t)
      return true;
  return false;
}

bool isKleeHandledIntrinsic(llvm::StringRef stripped_name) {
  if (stripped_name.empty())
    return false;
  static const llvm::StringSet<> kSet = [] {
    llvm::StringSet<> s;
    for (auto name : kKleeHandledIntrinsics)
      s.insert(name);
    return s;
  }();
  return kSet.contains(stripped_name);
}

} // namespace statekillerfinder

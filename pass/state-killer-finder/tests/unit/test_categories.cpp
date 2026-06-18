#include "StateKillerFinder.h"
#include <iostream>
#include <symbolon/TestCheck.h>

int main() {
  using namespace statekillerfinder;

  // Program terminators — named functions.
  SYMBOLON_CHECK(isProgramTerminator("abort"));
  SYMBOLON_CHECK(isProgramTerminator("exit"));
  SYMBOLON_CHECK(isProgramTerminator("_exit"));
  SYMBOLON_CHECK(isProgramTerminator("_Exit"));
  SYMBOLON_CHECK(isProgramTerminator("__assert"));
  SYMBOLON_CHECK(isProgramTerminator("__assert_fail"));
  SYMBOLON_CHECK(isProgramTerminator("__assert_rtn"));
  SYMBOLON_CHECK(isProgramTerminator("_assert"));
  SYMBOLON_CHECK(isProgramTerminator("klee_abort"));
  SYMBOLON_CHECK(isProgramTerminator("klee_silent_exit"));
  SYMBOLON_CHECK(isProgramTerminator("klee_report_error"));

  // Trap intrinsics are program terminators.
  SYMBOLON_CHECK(isProgramTerminator("llvm.trap"));
  SYMBOLON_CHECK(isProgramTerminator("llvm.debugtrap"));

  // Non-terminators.
  SYMBOLON_CHECK(!isProgramTerminator("printf"));
  SYMBOLON_CHECK(!isProgramTerminator("puts"));
  SYMBOLON_CHECK(!isProgramTerminator(""));

  // KLEE-handled intrinsics (suffix-stripped names).
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.memcpy"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.memset"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.memmove"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.fabs"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.fma"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.smax"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.dbg.declare"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.lifetime.start"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.lifetime.end"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.va_start"));
  SYMBOLON_CHECK(isKleeHandledIntrinsic("llvm.sadd.with.overflow"));

  // Trap intrinsics are NOT in the KLEE-handled-intrinsic allowlist
  // (they're routed to ProgramTerminator instead).
  SYMBOLON_CHECK(!isKleeHandledIntrinsic("llvm.trap"));
  SYMBOLON_CHECK(!isKleeHandledIntrinsic("llvm.debugtrap"));

  // Unhandled intrinsics.
  SYMBOLON_CHECK(!isKleeHandledIntrinsic("llvm.bitreverse"));
  SYMBOLON_CHECK(!isKleeHandledIntrinsic("llvm.cos"));
  SYMBOLON_CHECK(!isKleeHandledIntrinsic("llvm.sin"));

  // Empty input is rejected.
  SYMBOLON_CHECK(!isKleeHandledIntrinsic(""));

  std::cout << "test_categories: ok\n";
  return 0;
}

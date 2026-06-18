// state-killer-finder: --enable=UnsupportedInstruction
//
// On x86_64, clang lowers va_arg(ap, int) to struct-field accesses through
// llvm.va_* intrinsics (which KLEE handles) rather than emitting the LLVM
// VAArgInst opcode. This fixture verifies that the resulting bitcode does
// NOT trigger an UnsupportedInstruction false positive: the visitor's
// visitVAArgInst method exists as a portability hedge for targets that
// DO emit VAArgInst (e.g. some 32-bit ABIs), but on the runner's native
// platform it stays silent.
#include <stdarg.h>
int sum(int n, ...) {
  va_list ap;
  va_start(ap, n);
  int total = 0;
  for (int i = 0; i < n; ++i)
    total += va_arg(ap, int);
  va_end(ap);
  return total;
}
int main(void) { return sum(2, 3, 4); }

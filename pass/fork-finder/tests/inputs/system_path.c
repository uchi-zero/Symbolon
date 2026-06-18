// fork-finder: --include-non-loop
// Relies on glibc's inline bswap_32 body so MemOp findings carry debug
// locations under /nix/store/.../glibc/.../bits/byteswap.h, exercising
// the default system-path skip. Will not work on musl/uclibc, where the
// builtin lowering may bypass the heuristic.
#include <byteswap.h>
#include <stdint.h>

int announce(int x) {
  if (x > 0)
    return bswap_32((uint32_t)x);
  return 0;
}

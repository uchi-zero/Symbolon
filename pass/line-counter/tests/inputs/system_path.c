// line-counter: --include-system
// Calls bswap_32 from glibc's <byteswap.h> so the bitcode contains
// instructions whose !dbg points at /nix/store/.../bits/byteswap.h.
// Without --include-system that file is filtered out; with it, both
// system_path.c and the glibc header appear in the report.
#include <byteswap.h>
#include <stdint.h>

int announce(int x) {
  if (x > 0)
    return bswap_32((uint32_t)x);
  return 0;
}

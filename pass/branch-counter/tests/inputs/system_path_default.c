// branch-counter:
// Calls stdc_leading_zeros_ull from <stdbit.h>, which has an
// __always_inline function with a ternary -> branch whose debug-info
// points at the nix-store copy of stdbit.h (a system path).
// Without the filter, that header appears as a second file.
// With the filter on (default), only this .c file should appear.
#include <stdbit.h>
#include <stdint.h>

int count_lz(uint64_t x) {
  if (x > 0)
    return (int)stdc_leading_zeros_ull(x);
  return 64;
}

// fork-finder: --include-implicit --include-non-loop
#include <stdbool.h>
bool sum_safe(int a, int b, int *out) {
  return __builtin_add_overflow(a, b, out);
}

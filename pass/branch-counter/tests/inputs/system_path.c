// branch-counter: --include-system
// Same surface as system_path_default.c, but with the directive enabling
// --include-system. The stdbit.h inline branch should now appear too.
#include <stdbit.h>

int announce(int x) {
  if (x > 0)
    return (int)stdc_leading_zeros_ull((unsigned long long)x);
  return 0;
}

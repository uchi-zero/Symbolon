// fork-finder:
#include <string.h>
int find_run(const char *p, const char *q, unsigned len, unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    if (memcmp(p + i, q, len) == 0)
      return (int)i;
  }
  return -1;
}

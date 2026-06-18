// fork-finder:
#include <string.h>
int eq(const char *a, const char *b, unsigned n) {
  return memcmp(a, b, n) == 0;
}

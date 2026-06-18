// fork-finder:
#include <stdlib.h>
char *grow(char *p, unsigned *sizes, unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    p = realloc(p, sizes[i]);
  }
  return p;
}

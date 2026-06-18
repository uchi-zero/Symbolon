// fork-finder:
#include <stdlib.h>
char **make_buffers(unsigned *sizes, unsigned n) {
  char **bufs = malloc(n * sizeof(char *));
  for (unsigned i = 0; i < n; ++i) {
    bufs[i] = malloc(sizes[i]);
  }
  return bufs;
}

// fork-finder:
#include <stdlib.h>
void free_all(char **bufs, unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    free(bufs[i]);
  }
}

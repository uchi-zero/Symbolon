// fork-finder: --include-non-loop
#include <stdlib.h>
void *grab_aligned(int n) { return aligned_alloc(16, (size_t)n); }

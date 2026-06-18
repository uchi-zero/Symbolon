// fork-finder: --include-non-loop
#include <malloc.h>
void *grab_memalign(int n) { return memalign(16, (size_t)n); }

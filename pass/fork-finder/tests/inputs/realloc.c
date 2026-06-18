// fork-finder: --include-non-loop
#include <stdlib.h>
void *resize(void *p, int n) { return realloc(p, n); }

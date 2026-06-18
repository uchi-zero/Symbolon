// fork-finder:
#include <stdlib.h>
void *grab_aligned8(void) { return aligned_alloc(16, 64); }

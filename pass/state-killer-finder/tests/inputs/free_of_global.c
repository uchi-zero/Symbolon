// state-killer-finder: --enable=FreeOfNonHeap
#include <stdlib.h>
static int g = 7;
int main(void) {
  free(&g); // KLEE: "free of global"
  return 0;
}

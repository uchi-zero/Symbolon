// state-killer-finder: --enable=FreeOfNonHeap
#include <stdlib.h>
int main(void) {
  int local = 7;
  free(&local); // KLEE: "free of alloca"
  return 0;
}

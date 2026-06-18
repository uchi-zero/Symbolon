// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// Pin enough of the buffer to drive the OK branch in one path:
//   a[0..3] = "Host"  → memcmp succeeds
//   a[4]    = ':'     → memchr returns &a[4], so key_len == 4
// The cocci pin-string-search.cocci rule does NOT handle this pattern
// because its remedy pins ':' at a[0], which would make key_len == 0 and
// miss the OK branch entirely. When memchr's RESULT (not just its
// truthiness) is consumed, the pin must be tailored to the downstream
// check — that's what this hand-written remedy shows.
int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <input-file>\n", argv[0]);
    return 1;
  }
  FILE *fp = fopen(argv[1], "r");
  if (!fp) {
    perror("fopen");
    return 1;
  }
  char a[40];
  size_t n = fread(a, 1, sizeof(a), fp);
  printf("read %zu bytes\n", n);
  fclose(fp);
  int indicator = klee_range(0, 2, "indicator");
  if (indicator == 0) {
    klee_assume(a[0] == 'H');
    klee_assume(a[1] == 'o');
    klee_assume(a[2] == 's');
    klee_assume(a[3] == 't');
    klee_assume(a[4] == ':');
    char *colon = memchr(a, ':', n);
    if (colon != NULL) {
      size_t key_len = (size_t)(colon - a);
      if (key_len == 4 && memcmp(a, "Host", 4) == 0) {
        printf("OK\n");
      }
    }
  }
  return 0;
}

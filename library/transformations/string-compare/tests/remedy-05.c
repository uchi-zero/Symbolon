// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <strings.h>

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
    klee_assume((a[0] == 'H') | (a[0] == 'h'));
    klee_assume((a[1] == 'E') | (a[1] == 'e'));
    klee_assume((a[2] == 'L') | (a[2] == 'l'));
    klee_assume((a[3] == 'L') | (a[3] == 'l'));
    klee_assume((a[4] == 'O') | (a[4] == 'o'));
    klee_assume(a[5] == '-');
    klee_assume((a[6] == 'W') | (a[6] == 'w'));
    klee_assume((a[7] == 'O') | (a[7] == 'o'));
    klee_assume((a[8] == 'R') | (a[8] == 'r'));
    klee_assume((a[9] == 'L') | (a[9] == 'l'));
    klee_assume((a[10] == 'D') | (a[10] == 'd'));
    printf("OK\n");
  }
  return 0;
}

// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

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
  size_t n = fread(a, 1, sizeof(a) - 1, fp);
  a[n] = '\0';
  printf("read %zu bytes\n", n);
  fclose(fp);
  int indicator = klee_range(0, 2, "indicator");
  if (indicator == 0) {
    klee_assume(a[0] == '\0');
    if (strlen(a) < 16) {
      printf("OK\n");
    }
  }
  return 0;
}

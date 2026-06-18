// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// Parse an HTTP-header-like line "Name: value". Locate the ':' delimiter
// with memchr, derive the key length from its position, and check whether
// the key is "Host". Path explosion is compounded:
//   - memchr forks per candidate position of ':' in the buffer
//   - the derived key_len varies per memchr result
//   - the subsequent memcmp on the key forks byte-by-byte
// Together this produces hundreds of paths from a single header check.
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
  char *colon = memchr(a, ':', n);
  if (colon != NULL) {
    size_t key_len = (size_t)(colon - a);
    if (key_len == 4 && memcmp(a, "Host", 4) == 0) {
      printf("OK\n");
    }
  }
  return 0;
}

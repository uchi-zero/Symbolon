// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// Pin the buffer so the first byte IS in the reject set ("\r\n"),
// forcing strcspn to return 0. The truthy branch of `if (k < 16)` is
// reachable in a single concrete path; the strcspn call itself runs
// without forking because the first byte is constrained.
//
// Trade-off (semantics-altering): we only exercise the "line ends
// immediately" case. Coverage of the "line is K characters long for
// various K" cases is lost. For test-generation that just needs SOME
// concrete input that reaches the truthy branch, this is fine.

int main(int argc, char **argv) {
  if (argc < 2)
    return 1;
  FILE *fp = fopen(argv[1], "r");
  if (!fp)
    return 1;
  char buf[40];
  size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
  buf[n] = '\0';
  fclose(fp);

  int indicator = klee_range(0, 2, "indicator");
  if (indicator == 0) {
    klee_assume(buf[0] == '\n');
    if (strcspn(buf, "\r\n") < 16) {
      printf("short line\n");
    }
  }
  return 0;
}

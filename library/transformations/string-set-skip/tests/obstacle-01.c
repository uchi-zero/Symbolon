// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// strcspn over a symbolic buffer scans byte-by-byte until it finds a
// character in the reject set, then returns the offset. With symbolic
// input, this forks once per position checking each byte against the
// (small, fixed) reject set. For a 40-byte buffer and a 3-char reject
// set, that's ~120 fork combinations from the call alone.
//
// nasm uses this pattern in several places — see nasm.c:1410
// (`strcspn(buffer, "\r\n\032")` to find end-of-line), directiv.c:99
// (`strcspn(p, " ,")` to scan a name token), outbin.c:1109
// (`strcspn(*line, " \t")` to skip non-whitespace).

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

  if (strcspn(buf, "\r\n") < 16) {
    printf("short line\n");
  }
  return 0;
}

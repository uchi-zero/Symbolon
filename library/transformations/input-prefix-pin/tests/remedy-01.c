// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// Pin the first 14 bytes of the symbolic input to the known-valid
// parser prefix. KLEE skips byte-by-byte classification of those bytes
// (each pinned byte is a single equality constraint, not a fork) and
// dispatch goes straight to the deep parser branch.
//
// Trade-off (semantics-altering): only the "input is a valid Makefile
// fragment of this exact shape" execution is explored. Coverage of
// the parser's error-handling paths is forfeit. For test generation
// targeting the deep build engine, that's the right trade.

static void parse(const char *p) {
  if (p[0] != 'a')
    return;
  if (p[1] != 'l')
    return;
  if (p[2] != 'l')
    return;
  if (p[3] != ':')
    return;
  if (p[4] != '\n')
    return;
  if (p[5] != '\t')
    return;
  if (p[6] != 'e')
    return;
  if (p[7] != 'c')
    return;
  if (p[8] != 'h')
    return;
  if (p[9] != 'o')
    return;
  if (p[10] != ' ')
    return;
  if (p[11] != 'o')
    return;
  if (p[12] != 'k')
    return;
  if (p[13] != '\n')
    return;
  printf("OK\n");
}

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

  // Input-prefix pin: drive the parser into the deep branch in one
  // path instead of exploring per-byte alternatives at each dispatch.
  klee_assume(buf[0] == 'a');
  klee_assume(buf[1] == 'l');
  klee_assume(buf[2] == 'l');
  klee_assume(buf[3] == ':');
  klee_assume(buf[4] == '\n');
  klee_assume(buf[5] == '\t');
  klee_assume(buf[6] == 'e');
  klee_assume(buf[7] == 'c');
  klee_assume(buf[8] == 'h');
  klee_assume(buf[9] == 'o');
  klee_assume(buf[10] == ' ');
  klee_assume(buf[11] == 'o');
  klee_assume(buf[12] == 'k');
  klee_assume(buf[13] == '\n');

  parse(buf);
  return 0;
}

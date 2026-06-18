// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// A minimal recursive-descent parser dispatching on the first few bytes
// of symbolic input. This is the parser-shape obstacle we observed when
// running KLEE on GNU make: the symbolic file's 40 bytes never satisfy
// the parser's tokenization rules tightly enough for KLEE to make
// progress past the byte-classification phase. KLEE's budget is burned
// classifying single bytes; the deeper parser/dependency-resolver code
// is unreachable.
//
// The toy below has 7 dispatch branches keyed on input[0]. Without an
// input-shape pin KLEE explores all 7 — most of which are short
// error paths — but never combines specific dispatch choices for
// `input[0..N-1]` in a way that drives the deep "rule + recipe" path
// to "OK".

static void parse(const char *p) {
  // Recognise the Makefile-fragment shape "all:\n\techo ok\n".
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
  parse(buf);
  return 0;
}

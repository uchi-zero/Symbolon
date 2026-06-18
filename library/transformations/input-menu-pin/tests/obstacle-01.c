// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// A dispatcher with several deep "command" handlers, each reached only by a
// specific multi-byte command word and then running distinct deep code. This
// is the multi-feature parser-shape obstacle: a calculator/interpreter whose
// execution engine has many sibling handlers (add / mul / div / pow / ...),
// each gated behind a *complete, valid* command.
//
// With fully-symbolic input KLEE burns its budget classifying the leading
// bytes; the vast majority of byte combinations match no command and take the
// shallow "unknown" return. Reaching ALL of the deep handlers in one run means
// independently assembling each exact command out of symbolic bytes, against a
// fork-exploding lexical layer — so within a fixed budget only a fraction of
// the handlers' deep code is actually executed.

static long deep(int seed) {
  // Distinct deep code per handler (a small loop so each handler body is
  // more than one line; the point is that these lines are reached only when
  // the matching command is assembled).
  long acc = seed;
  for (int i = 0; i < seed + 2; i++)
    acc = acc * 31 + i;
  return acc;
}

static long handle(const char *p) {
  if (!strcmp(p, "add"))
    return deep(1); // deep path A
  if (!strcmp(p, "mul"))
    return deep(2); // deep path M
  if (!strcmp(p, "div"))
    return deep(3); // deep path D
  if (!strcmp(p, "pow"))
    return deep(4); // deep path P
  if (!strcmp(p, "mod"))
    return deep(5); // deep path O
  if (!strcmp(p, "sqr"))
    return deep(6); // deep path Q
  return 0;         // unrecognized -> shallow
}

int main(int argc, char **argv) {
  if (argc < 2)
    return 1;
  FILE *fp = fopen(argv[1], "r");
  if (!fp)
    return 1;
  char buf[16];
  size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
  buf[n] = '\0';
  fclose(fp);
  return (int)handle(buf);
}

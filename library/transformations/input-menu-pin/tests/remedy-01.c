// klee: --posix-runtime --libc=uclibc A --sym-files 1 40
#include "klee/klee.h"
#include <stdio.h>
#include <string.h>

// input-menu-pin remedy: instead of pinning the symbolic input to ONE valid
// command (input-prefix-pin, which would reach a single handler per run), pin
// it to ONE OF A MENU of valid commands, selected by a symbolic klee_range.
//
// KLEE forks ONCE on the selector and explores every menu entry in a SINGLE
// run: each branch constrains the symbolic input to a complete valid command
// (so the emitted ktest replays that command on the native binary) and reaches
// its deep handler and returns. One run therefore covers ALL handlers with
// exactly N completed paths — no lexical fork-explosion, every klee_assume set
// is self-consistent within its branch, and the coverage is a single run's
// (never a cross-run profraw union).

static long deep(int seed) {
  long acc = seed;
  for (int i = 0; i < seed + 2; i++)
    acc = acc * 31 + i;
  return acc;
}

static long handle(const char *p) {
  if (!strcmp(p, "add"))
    return deep(1);
  if (!strcmp(p, "mul"))
    return deep(2);
  if (!strcmp(p, "div"))
    return deep(3);
  if (!strcmp(p, "pow"))
    return deep(4);
  if (!strcmp(p, "mod"))
    return deep(5);
  if (!strcmp(p, "sqr"))
    return deep(6);
  return 0;
}

static const char *const menu[] = {"add", "mul", "div", "pow", "mod", "sqr"};
enum { NMENU = sizeof(menu) / sizeof(menu[0]) };

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

  // Pin to one menu entry, chosen by a symbolic selector. KLEE forks once
  // (one branch per entry); each branch makes the input that exact command.
  unsigned sel = (unsigned)klee_range(0, NMENU, "menu_sel");
  const char *cmd = menu[0];
  for (unsigned k = 1; k < (unsigned)NMENU; k++)
    if (sel == k)
      cmd = menu[k]; // concrete pointer in each forked state
  unsigned len = (unsigned)strlen(cmd);
  if (len > n)
    len = (unsigned)n;
  for (unsigned i = 0; i < len; i++) {
    klee_assume(buf[i] == cmd[i]); // ktest replays this command
    buf[i] = cmd[i];               // concretize so the match is fork-free
  }
  buf[len] = '\0';

  return (int)handle(buf);
}

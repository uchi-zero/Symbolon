// state-killer-finder: --enable=IndirectBranch
#include <klee/klee.h>
int dispatch(int sel) {
  static void *const tbl[] = {&&L0, &&L1};
  goto *tbl[sel];
L0:
  return 100;
L1:
  return 200;
}
int main(void) {
  int sel;
  klee_make_symbolic(&sel, sizeof sel, "sel");
  return dispatch(sel);
}

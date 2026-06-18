// branch-counter:
// LLVM still emits an implicit default successor → outcomes = N+1.
int kind(int x) {
  int r = -1;
  switch (x) {
  case 1:
    r = 10;
    break;
  case 2:
    r = 20;
    break;
  }
  return r;
}

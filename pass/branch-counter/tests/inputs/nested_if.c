// branch-counter:
int classify(int x) {
  if (x > 0) {
    if (x > 10)
      return 2;
    return 1;
  }
  return 0;
}

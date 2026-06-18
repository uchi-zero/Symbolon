// fork-finder: --include-implicit
int sum_overflow(const int *xs, unsigned n) {
  int s = 0;
  for (unsigned i = 0; i < n; ++i) {
    int r;
    if (__builtin_sadd_overflow(s, xs[i], &r))
      return -1;
    s = r;
  }
  return s;
}

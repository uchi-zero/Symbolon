// fork-finder: --include-implicit
int sum_div(const int *xs, const int *ys, unsigned n) {
  int s = 0;
  for (unsigned i = 0; i < n; ++i) {
    s += xs[i] / ys[i];
  }
  return s;
}

// fork-finder:
int sum_pos(const int *v, unsigned n) {
  int s = 0;
  for (unsigned i = 0; i < n; ++i) {
    if (v[i] > 0)
      s += v[i];
  }
  return s;
}

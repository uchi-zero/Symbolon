// fork-finder:
typedef int (*op_t)(int);
int apply_all(const int *xs, unsigned n, op_t op) {
  int s = 0;
  for (unsigned i = 0; i < n; ++i) {
    s += op(xs[i]);
  }
  return s;
}

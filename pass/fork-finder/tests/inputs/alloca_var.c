// fork-finder: --include-non-loop
int sum_n(int n) {
  int buf[n]; // VLA — non-const alloca
  int s = 0;
  for (int i = 0; i < n; ++i) {
    buf[i] = i;
    s += buf[i];
  }
  return s;
}

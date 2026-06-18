// fork-finder:
int peak(const int *m, unsigned r, unsigned c) {
  int best = 0;
  for (unsigned i = 0; i < r; ++i) {
    for (unsigned j = 0; j < c; ++j) {
      if (m[i * c + j] > best)
        best = m[i * c + j];
    }
  }
  return best;
}

// fork-finder:
int fixed_buf(void) {
  int a[10]; // constant size — skipped
  for (int i = 0; i < 10; ++i)
    a[i] = i;
  return a[9];
}

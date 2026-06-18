// fork-finder:
int dispatch(const int *kinds, unsigned n) {
  int acc = 0;
  unsigned i = 0;
  while (i < n) {
    switch (kinds[i]) {
    case 0:
      acc += 1;
      break;
    case 1:
      acc += 2;
      break;
    case 2:
      acc += 4;
      break;
    default:
      acc -= 1;
      break;
    }
    ++i;
  }
  return acc;
}

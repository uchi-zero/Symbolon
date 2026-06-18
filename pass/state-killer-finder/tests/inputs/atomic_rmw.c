// state-killer-finder: --enable=UnsupportedInstruction
int main(void) {
  int x = 0;
  __atomic_fetch_add(&x, 1, __ATOMIC_SEQ_CST);
  return x;
}

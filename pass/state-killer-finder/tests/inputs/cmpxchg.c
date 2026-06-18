// state-killer-finder: --enable=UnsupportedInstruction
int main(void) {
  int x = 0, expected = 0, desired = 1;
  __atomic_compare_exchange(&x, &expected, &desired, 0, __ATOMIC_SEQ_CST,
                            __ATOMIC_SEQ_CST);
  return x;
}

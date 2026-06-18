// state-killer-finder: --enable=ProgramTerminator
int main(void) {
  __builtin_trap();
  return 0;
}

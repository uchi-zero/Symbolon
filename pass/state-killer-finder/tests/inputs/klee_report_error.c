// state-killer-finder: --enable=ProgramTerminator
void klee_report_error(const char *file, int line, const char *msg,
                       const char *suffix);
int main(void) {
  klee_report_error("a.c", 1, "boom", "user");
  return 0;
}

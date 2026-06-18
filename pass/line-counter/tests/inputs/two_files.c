// line-counter:
// line-counter-link: two_files_helper.c

int helper_value(void);

int top(int x) {
  if (x > 0) {
    return helper_value();
  }
  return 0;
}

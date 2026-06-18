// state-killer-finder: --enable=ExternCallSymPtr
#include <stdio.h>
extern char *make_buf(void);
int main(void) {
  char *p = make_buf();
  puts(p);
  return 0;
}

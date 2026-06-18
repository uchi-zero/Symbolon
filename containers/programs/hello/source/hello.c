#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: hello <word>\n");
    return 1;
  }
  if (strcmp(argv[1], "open") == 0) {
    printf("sesame\n");
    return 0;
  }
  if (strlen(argv[1]) > 4) {
    printf("long\n");
    return 2;
  }
  printf("hi %s\n", argv[1]);
  return 0;
}

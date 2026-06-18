#include "klee/klee.h"
#include <stdio.h>

// Figure 1 from Perry et al., ISSTA 2017 ("Accelerating Array Constraints
// in Symbolic Execution"). A 256-byte lookup table maps each possible
// input byte to its base64 value (or -1 if not a base64 character).
// The predicate `b64[k] >= 0` against a symbolic k forces the SMT solver
// to encode all 256 array values plus the index-to-offset axiom, even
// though only the integer-level fact "k ∈ {valid offsets}" matters.
static const signed char b64[256] = {
    [0 ... 255] = -1, [43] = 62,  [47] = 63,  [48] = 52,  [49] = 53,
    [50] = 54,        [51] = 55,  [52] = 56,  [53] = 57,  [54] = 58,
    [55] = 59,        [56] = 60,  [57] = 61,  [65] = 0,   [66] = 1,
    [67] = 2,         [68] = 3,   [69] = 4,   [70] = 5,   [71] = 6,
    [72] = 7,         [73] = 8,   [74] = 9,   [75] = 10,  [76] = 11,
    [77] = 12,        [78] = 13,  [79] = 14,  [80] = 15,  [81] = 16,
    [82] = 17,        [83] = 18,  [84] = 19,  [85] = 20,  [86] = 21,
    [87] = 22,        [88] = 23,  [89] = 24,  [90] = 25,  [97] = 26,
    [98] = 27,        [99] = 28,  [100] = 29, [101] = 30, [102] = 31,
    [103] = 32,       [104] = 33, [105] = 34, [106] = 35, [107] = 36,
    [108] = 37,       [109] = 38, [110] = 39, [111] = 40, [112] = 41,
    [113] = 42,       [114] = 43, [115] = 44, [116] = 45, [117] = 46,
    [118] = 47,       [119] = 48, [120] = 49, [121] = 50, [122] = 51,
};

int main(void) {
  unsigned char k;
  klee_make_symbolic(&k, sizeof(k), "k");
  if (b64[k] >= 0) {
    printf("base64\n");
  }
  return 0;
}

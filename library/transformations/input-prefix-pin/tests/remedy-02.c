// remedy-02.c — input-prefix-pin applied to the ELF header gate.
//
// After the header bytes are made symbolic, pin a *coherent structural
// skeleton*: the magic, the EI_CLASS/EI_DATA/EI_VERSION dispatch bytes, and
// the program-header table layout (e_phoff = 32, e_phnum = 1). The bounds
// check and the loop trip count are now concrete, so KLEE drives one state
// straight into dispatch_segment instead of forking over offset/count
// combinations.
//
// Crucially the segment *type* word (p_type, at buf[32..35]) is left
// symbolic, so KLEE still forks the type switch and covers every PT_* case.
// This is the binary-format analogue of pinning "all:\n\techo ok\n" for
// make: fix the parse structure, keep the semantic tail symbolic.
#include "klee/klee.h"
#include <stdint.h>
#include <stdio.h>

#define N 256
static unsigned char buf[N];

static uint32_t rd32(int off) {
  return buf[off] | (buf[off + 1] << 8) | (buf[off + 2] << 16) |
         ((uint32_t)buf[off + 3] << 24);
}

static void dispatch_segment(uint32_t p_type) {
  switch (p_type) {
  case 0:
    printf("PT_NULL\n");
    break;
  case 1:
    printf("PT_LOAD\n");
    break;
  case 2:
    printf("PT_DYNAMIC\n");
    break;
  case 3:
    printf("PT_INTERP\n");
    break;
  case 4:
    printf("PT_NOTE\n");
    break;
  case 6:
    printf("PT_PHDR\n");
    break;
  case 7:
    printf("PT_TLS\n");
    break;
  default:
    printf("PT_UNKNOWN\n");
    break;
  }
}

int main(void) {
  klee_make_symbolic(buf, sizeof(buf), "elf");

  // Input-prefix pin: structurally-valid ELF32 skeleton.
  klee_assume(buf[0] == 0x7f);
  klee_assume(buf[1] == 'E');
  klee_assume(buf[2] == 'L');
  klee_assume(buf[3] == 'F');
  klee_assume(buf[4] == 1); // ELFCLASS32
  klee_assume(buf[5] == 1); // ELFDATA2LSB
  klee_assume(buf[6] == 1); // EV_CURRENT
  // e_phoff = 32
  klee_assume(buf[28] == 32);
  klee_assume(buf[29] == 0);
  klee_assume(buf[30] == 0);
  klee_assume(buf[31] == 0);
  // e_phnum = 1
  klee_assume(buf[44] == 1);
  klee_assume(buf[45] == 0);

  // check_magic_number (now a single satisfiable path)
  if (buf[0] != 0x7f || buf[1] != 'E' || buf[2] != 'L' || buf[3] != 'F')
    return 1;
  if (buf[4] != 1 && buf[4] != 2)
    return 1;
  if (buf[5] != 1 && buf[5] != 2)
    return 1;
  if (buf[6] != 1)
    return 1;

  uint32_t e_phoff = rd32(28);
  uint16_t e_phnum = buf[44] | (buf[45] << 8);

  if ((uint64_t)e_phoff + (uint64_t)e_phnum * 32 > N)
    return 1;

  for (uint16_t i = 0; i < e_phnum; i++) {
    uint32_t off = e_phoff + i * 32;
    dispatch_segment(rd32(off)); // p_type still symbolic -> covers all cases
  }
  return 0;
}

// obstacle-02.c — ELF header gate (models readelf's get_file_header +
// process_program_headers). The whole header is symbolic: not only the
// 4-byte magic and the class/data/version bytes, but also the structural
// fields e_phoff (program-header table offset) and e_phnum (entry count).
//
// With e_phoff and e_phnum symbolic, the bounds check and the table loop
// fork on a huge domain (e_phoff is a symbolic 32-bit value, e_phnum a
// symbolic 16-bit count), so KLEE burns its budget enumerating offset/count
// combinations and rarely drives a state into the per-segment type switch
// (dispatch_segment) that is the code we actually want covered.
//
// This mirrors readelf on a 100-byte symbolic file: KLEE solves the magic
// on its own, but the symbolic e_phoff/e_phnum/e_shoff/e_shnum keep it
// fork-bound in the table-walking layer instead of the type-dispatch layer.
#include "klee/klee.h"
#include <stdint.h>
#include <stdio.h>

#define N 256
static unsigned char buf[N];

static uint32_t rd32(int off) {
  return buf[off] | (buf[off + 1] << 8) | (buf[off + 2] << 16) |
         ((uint32_t)buf[off + 3] << 24);
}

// The semantic core we want covered: dispatch on the segment type.
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

  // check_magic_number
  if (buf[0] != 0x7f || buf[1] != 'E' || buf[2] != 'L' || buf[3] != 'F')
    return 1;
  if (buf[4] != 1 && buf[4] != 2)
    return 1; // EI_CLASS: ELFCLASS32/64
  if (buf[5] != 1 && buf[5] != 2)
    return 1; // EI_DATA:  ELFDATA2LSB/MSB
  if (buf[6] != 1)
    return 1; // EI_VERSION: EV_CURRENT

  // ELF32 structural fields: e_phoff @ 28, e_phnum @ 44 (little-endian).
  uint32_t e_phoff = rd32(28);
  uint16_t e_phnum = buf[44] | (buf[45] << 8);

  // readelf-style bounds check on the program-header table.
  if ((uint64_t)e_phoff + (uint64_t)e_phnum * 32 > N)
    return 1;

  for (uint16_t i = 0; i < e_phnum; i++) {
    uint32_t off = e_phoff + i * 32;
    dispatch_segment(rd32(off)); // p_type @ start of each program header
  }
  return 0;
}

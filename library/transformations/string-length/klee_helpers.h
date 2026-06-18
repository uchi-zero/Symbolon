#ifndef KLEE_HELPERS_LEN_H
#define KLEE_HELPERS_LEN_H

#include "klee/klee.h"
#include <stddef.h>

// Pin the buffer to length 0 (its first byte is the null terminator).
// Used as a replacement for strlen/strnlen checks of the form
// `< N`, `<= N`, `== 0`, `! strlen(...)` — anywhere a zero-length string
// satisfies the predicate. Drives one concrete path through the
// strlen call: strlen returns 0 without forking.
static inline void klee_assume_strlen_zero(const char *buf) {
  klee_assume(buf[0] == '\0');
}

// Pin the buffer to a single-character string. After this, strlen returns
// 1. Used for predicates like `> 0`, `>= 1`, `== 1`.
static inline void klee_assume_strlen_one(const char *buf, char c) {
  klee_assume(buf[0] == c);
  klee_assume(buf[1] == '\0');
}

#endif

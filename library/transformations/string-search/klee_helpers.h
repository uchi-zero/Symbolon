#ifndef KLEE_HELPERS_SEARCH_H
#define KLEE_HELPERS_SEARCH_H

#include "klee/klee.h"
#include <stddef.h>

// Boolean-context replacements for the position-search family. Each forces
// a match at position 0 of the buffer, which is sufficient to drive any
// `if (strstr(...) / strchr(...) / ...)` check through the truthy branch.
// The literal/position is concrete inside each helper, so KLEE adds the
// constraints without forking.

// Pin buf[0..strlen(lit)-1] to lit (no null terminator pin — strstr only
// requires the substring to appear somewhere).
static inline void klee_assume_starts_with(const char *buf, const char *lit) {
  size_t i = 0;
  while (lit[i]) {
    klee_assume(buf[i] == lit[i]);
    i++;
  }
}

// Same as klee_assume_starts_with but with an explicit length (for memmem).
static inline void klee_assume_starts_with_bytes(const char *buf,
                                                 const char *lit, size_t n) {
  for (size_t i = 0; i < n; i++)
    klee_assume(buf[i] == lit[i]);
}

// Pin buf[0] == c (for strchr / memchr).
static inline void klee_assume_has_char(const char *buf, int c) {
  klee_assume((unsigned char)buf[0] == (unsigned char)c);
}

// Pin buf[0] to the first character in set (for strpbrk).
static inline void klee_assume_has_any(const char *buf, const char *set) {
  klee_assume(buf[0] == set[0]);
}

#endif

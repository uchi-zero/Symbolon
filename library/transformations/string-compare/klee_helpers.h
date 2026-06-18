#ifndef KLEE_HELPERS_H
#define KLEE_HELPERS_H

#include "klee/klee.h"
#include <stddef.h>

// Add one equality constraint per byte: buf[i] == lit[i] for i in [0, n).
// The loop bound is concrete, so KLEE unrolls without forking.
// Used as a path-explosion-free replacement for memcmp/strncmp/bcmp checks
// against a fixed literal.
static inline void klee_assume_eq_bytes(const char *buf, const char *lit,
                                        size_t n) {
  for (size_t i = 0; i < n; i++)
    klee_assume(buf[i] == lit[i]);
}

// Same as klee_assume_eq_bytes but also pins the null terminator at the
// position where the literal ends. Used as a replacement for strcmp checks.
static inline void klee_assume_eq_cstr(const char *buf, const char *lit) {
  size_t i = 0;
  while (lit[i]) {
    klee_assume(buf[i] == lit[i]);
    i++;
  }
  klee_assume(buf[i] == '\0');
}

// Case-insensitive equivalents: at each byte position, allow either case
// of the literal's letter (and exact match for non-letters). Uses bitwise
// OR (not logical ||) so the constraint is a single SMT expression rather
// than a branch — no fork per byte.
// Replacement for strncasecmp checks.
static inline void klee_assume_eqi_bytes(const char *buf, const char *lit,
                                         size_t n) {
  for (size_t i = 0; i < n; i++) {
    unsigned char c = (unsigned char)lit[i];
    unsigned char lo = (c >= 'A' && c <= 'Z') ? (c | 0x20u) : c;
    unsigned char hi = (c >= 'a' && c <= 'z') ? (c & ~0x20u) : c;
    klee_assume(((unsigned char)buf[i] == lo) | ((unsigned char)buf[i] == hi));
  }
}

// Replacement for strcasecmp checks.
static inline void klee_assume_eqi_cstr(const char *buf, const char *lit) {
  size_t i = 0;
  while (lit[i]) {
    unsigned char c = (unsigned char)lit[i];
    unsigned char lo = (c >= 'A' && c <= 'Z') ? (c | 0x20u) : c;
    unsigned char hi = (c >= 'a' && c <= 'z') ? (c & ~0x20u) : c;
    klee_assume(((unsigned char)buf[i] == lo) | ((unsigned char)buf[i] == hi));
    i++;
  }
  klee_assume(buf[i] == '\0');
}

#endif

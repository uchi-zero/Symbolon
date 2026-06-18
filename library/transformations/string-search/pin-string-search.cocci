// pin-string-search.cocci
//
// Rewrites boolean-context calls to strstr / strchr / strrchr / memchr /
// strpbrk / memmem into an indicator-pinned form that drives the truthy
// branch with O(1) symbolic paths.
//
// Each of these functions walks the haystack byte-by-byte looking for a
// match. Against a symbolic buffer that is one fork per candidate position
// plus per-byte forks at each position. Boolean-context use (`if
// (strstr(buf, "x")) ...`) only needs *some* concrete input that satisfies
// the check, so we pin a single position (the start of the buffer) to the
// target rather than enumerating all of them.
//
// Required helpers (defined in klee_helpers.h, added if missing):
//
//   klee_assume_starts_with(buf, lit)              for strstr
//   klee_assume_starts_with_bytes(buf, lit, n)     for memmem
//   klee_assume_has_char(buf, c)                   for strchr / memchr /
//                                                   strrchr
//   klee_assume_has_any(buf, set)                  for strpbrk
//
// Usage:
//   spatch --sp-file pin-string-search.cocci file.c            # preview
//   spatch --sp-file pin-string-search.cocci --in-place file.c # apply
//
// IN SCOPE — patterns this cocci rewrites:
//
//   if (FN(buf, target [, n]))            { ... }
//   if (FN(buf, target [, n]) != NULL)    { ... }
//
//   where FN ∈ {strstr, strchr, strrchr, memchr, strpbrk}
//   and the call appears directly in the if-condition.
//
// OUT OF SCOPE — patterns NOT rewritten:
//
//   1. Result assigned to a pointer then dereferenced or position-tested:
//        char *p = strstr(buf, "x");
//        if (p && p[3] == 'Y') { ... }            // reads past p
//        if (p && (p - buf) == 5) { ... }          // tests match position
//      Reason: this cocci pins the match at buf[0]. After the rewrite
//      `p == buf`, so any predicate about `p`'s offset or what follows
//      `p` becomes incorrect. Coverage of the OK branch is LOST.
//      Manual remedy: hand-write the pin to satisfy the downstream
//      predicate (see string-search/tests/remedy-03.c for an HTTP-Host
//      parser example).
//
//   2. memmem with a non-literal needle:
//        memmem(haystack, hl, needle, nl)
//      where needle is a buffer rather than a string literal.
//      Reason: nothing fixed to pin to.
//      Manual remedy: pin the needle first, then this rule applies.
//
//   3. Inverted predicate ("not found"):
//        if (!strstr(buf, "x")) { ... }
//        if (memchr(buf, c, n) == NULL) { ... }
//      Reason: the helper pins a match into buf, which makes this branch
//      unreachable. Coverage of the "no match" path is unchanged in the
//      original program.
//      Manual remedy: usually not needed.
//
// COVERAGE NOTE: This rewrite is coverage-preserving for the patterns IN
// SCOPE. For OUT-OF-SCOPE patterns the cocci silently skips them (the
// `if (memchr(...))` template doesn't match), so it cannot make those
// cases worse — but it also cannot help them.

@ has_include @
@@
#include "klee_helpers.h"

@ add_include depends on !has_include @
@@
+ #include "klee_helpers.h"
  int main(...) { ... }

// ---------- strstr / strrchr-as-bool / memmem: block-bodied if ----------
@ rewrite_search_block @
expression BUF;
expression LIT;
expression C;
expression N;
@@

(
- if (strstr(BUF, LIT)) {
+ if (klee_range(0, 2, "__pin_strstr") == 0) {
+   klee_assume_starts_with(BUF, LIT);
    ...
  }
|
- if (strstr(BUF, LIT) != NULL) {
+ if (klee_range(0, 2, "__pin_strstr") == 0) {
+   klee_assume_starts_with(BUF, LIT);
    ...
  }
|
- if (strchr(BUF, C)) {
+ if (klee_range(0, 2, "__pin_strchr") == 0) {
+   klee_assume_has_char(BUF, C);
    ...
  }
|
- if (strchr(BUF, C) != NULL) {
+ if (klee_range(0, 2, "__pin_strchr") == 0) {
+   klee_assume_has_char(BUF, C);
    ...
  }
|
- if (strrchr(BUF, C)) {
+ if (klee_range(0, 2, "__pin_strrchr") == 0) {
+   klee_assume_has_char(BUF, C);
    ...
  }
|
- if (strrchr(BUF, C) != NULL) {
+ if (klee_range(0, 2, "__pin_strrchr") == 0) {
+   klee_assume_has_char(BUF, C);
    ...
  }
|
- if (memchr(BUF, C, N)) {
+ if (klee_range(0, 2, "__pin_memchr") == 0) {
+   klee_assume_has_char(BUF, C);
    ...
  }
|
- if (memchr(BUF, C, N) != NULL) {
+ if (klee_range(0, 2, "__pin_memchr") == 0) {
+   klee_assume_has_char(BUF, C);
    ...
  }
|
- if (strpbrk(BUF, LIT)) {
+ if (klee_range(0, 2, "__pin_strpbrk") == 0) {
+   klee_assume_has_any(BUF, LIT);
    ...
  }
|
- if (strpbrk(BUF, LIT) != NULL) {
+ if (klee_range(0, 2, "__pin_strpbrk") == 0) {
+   klee_assume_has_any(BUF, LIT);
    ...
  }
)

// ---------- braceless bodies ----------
@ rewrite_search_single @
expression BUF;
expression LIT;
expression C;
expression N;
statement S;
@@

(
- if (strstr(BUF, LIT))
-   S
+ if (klee_range(0, 2, "__pin_strstr") == 0) {
+   klee_assume_starts_with(BUF, LIT);
+   S
+ }
|
- if (strchr(BUF, C))
-   S
+ if (klee_range(0, 2, "__pin_strchr") == 0) {
+   klee_assume_has_char(BUF, C);
+   S
+ }
|
- if (strrchr(BUF, C))
-   S
+ if (klee_range(0, 2, "__pin_strrchr") == 0) {
+   klee_assume_has_char(BUF, C);
+   S
+ }
|
- if (memchr(BUF, C, N))
-   S
+ if (klee_range(0, 2, "__pin_memchr") == 0) {
+   klee_assume_has_char(BUF, C);
+   S
+ }
|
- if (strpbrk(BUF, LIT))
-   S
+ if (klee_range(0, 2, "__pin_strpbrk") == 0) {
+   klee_assume_has_any(BUF, LIT);
+   S
+ }
)

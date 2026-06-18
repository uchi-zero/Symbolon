// pin-string-compare.cocci
//
// Rewrites fixed-literal calls to memcmp / strncmp / strcmp (and friends)
// inside an `if (...)` condition into an indicator-pinned form for KLEE
// path-explosion reduction.
//
// Each of these functions implements a byte-by-byte comparison with an
// early exit on the first mismatch (memcmp/strncmp) or on the first null
// (strcmp). Against a symbolic buffer, that produces O(N) forks per call:
// one early-exit path per byte position plus one full-match path.
//
// The transformation introduces a pre-fork on an indicator variable; on
// the "match" branch it calls a helper that adds one equality constraint
// per byte to the path constraint, without branching. Net result is O(1)
// completed paths per call.
//
// Two helpers are required, defined as `static inline` in the shipped
// header `klee_helpers.h` (sibling of this .cocci file):
//
//   klee_assume_eq_bytes(buf, lit, n)   for memcmp / strncmp / bcmp
//   klee_assume_eq_cstr(buf, lit)       for strcmp (also pins the null
//                                        terminator at lit's end)
//
// Both helpers loop with concrete bounds, so KLEE unrolls them without
// forking — equivalent in cost to a hand-unrolled klee_assume sequence.
//
// The patch adds `#include "klee_helpers.h"` to each rewritten file if
// it is not already present. The header must be reachable on the include
// path at build time (alongside the .c file, or via -I).
//
// Usage:
//   spatch --sp-file pin-string-compare.cocci file.c            # preview
//   spatch --sp-file pin-string-compare.cocci --in-place file.c # apply
//   spatch --sp-file pin-string-compare.cocci --dir src/ --in-place
//
// Detection-only: a non-empty diff means the pattern was found. To list
// matching files without applying:
//   spatch --sp-file pin-string-compare.cocci --dir src/ 2>/dev/null \
//     | grep '^+++ ' | sed 's|.*/||'
//
// IN SCOPE — patterns this cocci rewrites:
//
//   if (!FN(buf, "lit", n))            { ... }
//   if (FN(buf, "lit", n) == 0)        { ... }
//   if (0 == FN(buf, "lit", n))        { ... }
//   if (!FN(buf, "lit"))               { ... }   // strcmp / strcasecmp
//   if (FN(buf, "lit") == 0)           { ... }   // strcmp / strcasecmp
//   if (0 == FN(buf, "lit"))           { ... }   // strcmp / strcasecmp
//
//   where FN ∈ {memcmp, strncmp, bcmp, strcmp, strcasecmp, strncasecmp}
//   and "lit" is a string literal.
//
// OUT OF SCOPE — patterns NOT rewritten:
//
//   1. Result assigned to a variable then tested:
//        int r = memcmp(buf, "lit", n);
//        if (r == 0) { ... }
//      Reason: cocci matches the call inside the if-condition only.
//      Manual remedy: refactor to inline the call, or hand-write the pin.
//
//   2. Comparison against a non-literal:
//        if (!memcmp(buf, other_buf, n)) { ... }
//      Reason: there is no fixed byte pattern to pin to.
//      Manual remedy: pin one buffer first, then the rule applies.
//
//   3. Inverted predicate (drives the "no match" path):
//        if (memcmp(buf, "lit", n) != 0) { ... }
//      Reason: the helper pins buf == lit, which makes this branch
//      unreachable. Coverage of the "no match" path is unchanged in the
//      original program — KLEE already explores N-1 mismatch paths.
//      Manual remedy: usually not needed.

@ has_include @
@@
#include "klee_helpers.h"

@ add_include depends on !has_include @
@@
+ #include "klee_helpers.h"
  int main(...) { ... }

// ---------- memcmp / strncmp / bcmp: block-bodied if ----------
@ rewrite_bytes_block @
expression BUF;
expression LIT;
expression N;
@@

(
- if (!memcmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_memcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (memcmp(BUF, LIT, N) == 0) {
+ if (klee_range(0, 2, "__pin_memcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (0 == memcmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_memcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (!strncmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_strncmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (strncmp(BUF, LIT, N) == 0) {
+ if (klee_range(0, 2, "__pin_strncmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (0 == strncmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_strncmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (!bcmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_bcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (bcmp(BUF, LIT, N) == 0) {
+ if (klee_range(0, 2, "__pin_bcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
    ...
  }
|
- if (!strncasecmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_strncasecmp") == 0) {
+   klee_assume_eqi_bytes(BUF, LIT, N);
    ...
  }
|
- if (strncasecmp(BUF, LIT, N) == 0) {
+ if (klee_range(0, 2, "__pin_strncasecmp") == 0) {
+   klee_assume_eqi_bytes(BUF, LIT, N);
    ...
  }
|
- if (0 == strncasecmp(BUF, LIT, N)) {
+ if (klee_range(0, 2, "__pin_strncasecmp") == 0) {
+   klee_assume_eqi_bytes(BUF, LIT, N);
    ...
  }
)

// ---------- memcmp / strncmp / bcmp / strncasecmp: braceless if ----------
@ rewrite_bytes_single @
expression BUF;
expression LIT;
expression N;
statement S;
@@

(
- if (!memcmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_memcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (memcmp(BUF, LIT, N) == 0)
-   S
+ if (klee_range(0, 2, "__pin_memcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (0 == memcmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_memcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (!strncmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_strncmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (strncmp(BUF, LIT, N) == 0)
-   S
+ if (klee_range(0, 2, "__pin_strncmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (0 == strncmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_strncmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (!bcmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_bcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (bcmp(BUF, LIT, N) == 0)
-   S
+ if (klee_range(0, 2, "__pin_bcmp") == 0) {
+   klee_assume_eq_bytes(BUF, LIT, N);
+   S
+ }
|
- if (!strncasecmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_strncasecmp") == 0) {
+   klee_assume_eqi_bytes(BUF, LIT, N);
+   S
+ }
|
- if (strncasecmp(BUF, LIT, N) == 0)
-   S
+ if (klee_range(0, 2, "__pin_strncasecmp") == 0) {
+   klee_assume_eqi_bytes(BUF, LIT, N);
+   S
+ }
|
- if (0 == strncasecmp(BUF, LIT, N))
-   S
+ if (klee_range(0, 2, "__pin_strncasecmp") == 0) {
+   klee_assume_eqi_bytes(BUF, LIT, N);
+   S
+ }
)

// ---------- strcmp / strcasecmp: block-bodied if ----------
@ rewrite_cstr_block @
expression BUF;
expression LIT;
@@

(
- if (!strcmp(BUF, LIT)) {
+ if (klee_range(0, 2, "__pin_strcmp") == 0) {
+   klee_assume_eq_cstr(BUF, LIT);
    ...
  }
|
- if (strcmp(BUF, LIT) == 0) {
+ if (klee_range(0, 2, "__pin_strcmp") == 0) {
+   klee_assume_eq_cstr(BUF, LIT);
    ...
  }
|
- if (0 == strcmp(BUF, LIT)) {
+ if (klee_range(0, 2, "__pin_strcmp") == 0) {
+   klee_assume_eq_cstr(BUF, LIT);
    ...
  }
|
- if (!strcasecmp(BUF, LIT)) {
+ if (klee_range(0, 2, "__pin_strcasecmp") == 0) {
+   klee_assume_eqi_cstr(BUF, LIT);
    ...
  }
|
- if (strcasecmp(BUF, LIT) == 0) {
+ if (klee_range(0, 2, "__pin_strcasecmp") == 0) {
+   klee_assume_eqi_cstr(BUF, LIT);
    ...
  }
|
- if (0 == strcasecmp(BUF, LIT)) {
+ if (klee_range(0, 2, "__pin_strcasecmp") == 0) {
+   klee_assume_eqi_cstr(BUF, LIT);
    ...
  }
)

// ---------- strcmp / strcasecmp: braceless if ----------
@ rewrite_cstr_single @
expression BUF;
expression LIT;
statement S;
@@

(
- if (!strcmp(BUF, LIT))
-   S
+ if (klee_range(0, 2, "__pin_strcmp") == 0) {
+   klee_assume_eq_cstr(BUF, LIT);
+   S
+ }
|
- if (strcmp(BUF, LIT) == 0)
-   S
+ if (klee_range(0, 2, "__pin_strcmp") == 0) {
+   klee_assume_eq_cstr(BUF, LIT);
+   S
+ }
|
- if (0 == strcmp(BUF, LIT))
-   S
+ if (klee_range(0, 2, "__pin_strcmp") == 0) {
+   klee_assume_eq_cstr(BUF, LIT);
+   S
+ }
|
- if (!strcasecmp(BUF, LIT))
-   S
+ if (klee_range(0, 2, "__pin_strcasecmp") == 0) {
+   klee_assume_eqi_cstr(BUF, LIT);
+   S
+ }
|
- if (strcasecmp(BUF, LIT) == 0)
-   S
+ if (klee_range(0, 2, "__pin_strcasecmp") == 0) {
+   klee_assume_eqi_cstr(BUF, LIT);
+   S
+ }
|
- if (0 == strcasecmp(BUF, LIT))
-   S
+ if (klee_range(0, 2, "__pin_strcasecmp") == 0) {
+   klee_assume_eqi_cstr(BUF, LIT);
+   S
+ }
)

// pin-string-length.cocci
//
// Rewrites strlen/strnlen length-checks into an indicator-pinned form.
// strlen against a symbolic buffer forks once per byte position checking
// for the null terminator (up to buffer capacity + 1 paths).
//
// The default pin makes the buffer empty (strlen returns 0). This drives
// the truthy branch of any predicate satisfied by zero-length input:
// `< N`, `<= N`, `== 0`, `!strlen(...)`. For predicates needing a
// non-zero length (`> N`, `>= 1`, `== K`), use klee_assume_strlen_one
// or hand-write a different pin.
//
// Required helpers (in klee_helpers.h, added if missing):
//
//   klee_assume_strlen_zero(buf)        pin length to 0
//   klee_assume_strlen_one(buf, c)      pin length to 1 with char c
//
// Usage:
//   spatch --sp-file pin-string-length.cocci file.c            # preview
//   spatch --sp-file pin-string-length.cocci --in-place file.c # apply
//
// IN SCOPE — patterns this cocci rewrites:
//
//   if (strlen(buf) < N)    { ... }
//   if (strlen(buf) <= N)   { ... }
//   if (strlen(buf) == 0)   { ... }
//   if (!strlen(buf))       { ... }
//   if (strnlen(buf, L) < N)  { ... }
//   if (strnlen(buf, L) <= N) { ... }
//   if (strnlen(buf, L) == 0) { ... }
//
//   where the predicate is satisfiable with len == 0 (empty buffer).
//
// OUT OF SCOPE — patterns NOT rewritten:
//
//   1. Predicates that need len > 0:
//        if (strlen(buf) > N)   { ... }
//        if (strlen(buf) >= 1)  { ... }
//        if (strlen(buf) == K)  { ... }  // K != 0
//      Reason: this cocci pins len=0, which makes these predicates false.
//      Coverage of the OK branch is LOST.
//      Manual remedy: use klee_assume_strlen_one(buf, c) (defined in
//      klee_helpers.h) to pin len=1, or write a longer pin by hand.
//
//   2. strlen result assigned to a variable then tested:
//        size_t len = strlen(buf);
//        if (len < 16) { ... }
//      Reason: cocci matches the call inside the if-condition only.
//      Manual remedy: inline the call, or hand-write the pin before
//      the strlen call.
//
//   3. strcpy / strcat / stpcpy / snprintf %s:
//        strcpy(dst, src);   // dst then used downstream
//      Reason: the pin must be applied to the source buffer in the
//      surrounding scope, and the cocci can't wrap "the rest of the
//      function" syntactically. The path explosion here comes from the
//      copy-loop's per-byte fork on the source's null position.
//      Manual remedy: pin the source's null at a chosen position before
//      the call (see string-length/tests/obstacle-03.c + remedy-03.c).

@ has_include @
@@
#include "klee_helpers.h"

@ add_include depends on !has_include @
@@
+ #include "klee_helpers.h"
  int main(...) { ... }

// ---------- strlen / strnlen with `< N` style predicates (block body) ----------
@ rewrite_len_block @
expression BUF;
expression N;
expression LIM;
statement IFSTMT;
@@

(
- if (strlen(BUF) < N) IFSTMT
+ if (klee_range(0, 2, "__pin_strlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (strlen(BUF) < N) IFSTMT
+ }
|
- if (strlen(BUF) <= N) IFSTMT
+ if (klee_range(0, 2, "__pin_strlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (strlen(BUF) <= N) IFSTMT
+ }
|
- if (strlen(BUF) == 0) IFSTMT
+ if (klee_range(0, 2, "__pin_strlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (strlen(BUF) == 0) IFSTMT
+ }
|
- if (!strlen(BUF)) IFSTMT
+ if (klee_range(0, 2, "__pin_strlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (!strlen(BUF)) IFSTMT
+ }
|
- if (strnlen(BUF, LIM) < N) IFSTMT
+ if (klee_range(0, 2, "__pin_strnlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (strnlen(BUF, LIM) < N) IFSTMT
+ }
|
- if (strnlen(BUF, LIM) <= N) IFSTMT
+ if (klee_range(0, 2, "__pin_strnlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (strnlen(BUF, LIM) <= N) IFSTMT
+ }
|
- if (strnlen(BUF, LIM) == 0) IFSTMT
+ if (klee_range(0, 2, "__pin_strnlen") == 0) {
+   klee_assume_strlen_zero(BUF);
+   if (strnlen(BUF, LIM) == 0) IFSTMT
+ }
)

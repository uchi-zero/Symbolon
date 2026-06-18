// pin-string-set-skip.cocci
//
// Rewrites strcspn / strspn calls with a literal set into an indicator-
// pinned form that drives the truthy branch with O(1) symbolic paths.
//
// strcspn(s, set) scans s byte-by-byte advancing while s[i] is NOT in
// set; strspn(s, set) advances while s[i] IS in set. Both return the
// length of the prefix. With symbolic s and a small literal set, each
// position forks once per character of the set (matched/not-matched).
//
// Found in nasm hotspots: asm/nasm.c:1410 (strcspn(buf, "\\r\\n\\032")
// for line-end), asm/directiv.c:99 (strcspn(p, " ,") for name token),
// output/outbin.c:1109/1130/1274/1277 (strspn/strcspn for whitespace
// skipping). Each call site contributes O(buffer_size * set_size)
// forks per parser invocation.
//
// REMEDY: pin the first byte of s so the function returns 0, which
// satisfies the most common surrounding check `if (FN(s, set) < N)`.
// For strcspn, pin s[0] to the first character of set (it's in the
// set, so the not-in-set scan terminates at offset 0). For strspn,
// pin s[0] to any character NOT in set (we use 0xFF as a value
// unlikely to appear in printable-ASCII sets).
//
// This trade-off (semantics-altering) is identical to the rest of the
// path-explosion family: we cover one concrete control-flow shape
// rather than enumerate all length values. For test-generation that
// just needs SOME concrete input reaching the body, this is the right
// trade.
//
// IN SCOPE — patterns this cocci REWRITES:
//
//   if (strcspn(buf, "lit") < N)   { ... }   // any "< N" predicate
//   if (strcspn(buf, "lit") <= N)  { ... }
//   if (strcspn(buf, "lit") == 0)  { ... }
//   if (!strcspn(buf, "lit"))      { ... }
//   if (strspn(buf, "lit") < N)    { ... }
//   if (strspn(buf, "lit") <= N)   { ... }
//   if (strspn(buf, "lit") == 0)   { ... }
//   if (!strspn(buf, "lit"))       { ... }
//
//   where "lit" is a string literal — non-literal sets need analysis
//   the cocci can't do.
//
// OUT OF SCOPE:
//
//   1. Result-assigned-to-variable:
//        size_t k = strcspn(buf, "lit");
//        if (k > N) { ... }
//      Reason: cocci only matches calls inside an if-condition. Inline
//      the call or write the pin by hand.
//
//   2. Predicates needing a NON-ZERO return:
//        if (strcspn(buf, "lit") > N)   // wants the prefix to be long
//      Reason: our pin returns 0, which fails `> N`. For the long-
//      prefix case, write a hand pin that constrains buf[0..N-1] to
//      not-in-set and buf[N] to in-set.
//
//   3. The advancing-pointer idiom:
//        line += strcspn(line, " \\t");   // common in tokenizers
//      Reason: cocci only matches if-conditions, not arithmetic
//      assignments. Inline the call or rewrite by hand.
//
// Usage:
//   spatch --sp-file pin-string-set-skip.cocci file.c            # preview
//   spatch --sp-file pin-string-set-skip.cocci --in-place file.c # apply

// ---------- strcspn: pin buf[0] to set[0] (an in-set character) ----------
@ rewrite_strcspn @
expression BUF;
expression SET;
expression N;
statement S;
@@

(
- if (strcspn(BUF, SET) < N) S
+ if (klee_range(0, 2, "__pin_strcspn") == 0) {
+   klee_assume(BUF[0] == SET[0]);
+   if (strcspn(BUF, SET) < N) S
+ }
|
- if (strcspn(BUF, SET) <= N) S
+ if (klee_range(0, 2, "__pin_strcspn") == 0) {
+   klee_assume(BUF[0] == SET[0]);
+   if (strcspn(BUF, SET) <= N) S
+ }
|
- if (strcspn(BUF, SET) == 0) S
+ if (klee_range(0, 2, "__pin_strcspn") == 0) {
+   klee_assume(BUF[0] == SET[0]);
+   if (strcspn(BUF, SET) == 0) S
+ }
|
- if (!strcspn(BUF, SET)) S
+ if (klee_range(0, 2, "__pin_strcspn") == 0) {
+   klee_assume(BUF[0] == SET[0]);
+   if (!strcspn(BUF, SET)) S
+ }
)

// ---------- strspn: pin buf[0] to 0xFF (an out-of-set character) ----------
// Caveat: only correct if the set doesn't include 0xFF. For ASCII-only
// sets (whitespace, digits, punctuation, etc.) this is safe. For sets
// containing arbitrary bytes, hand-pick a different out-of-set value.
@ rewrite_strspn @
expression BUF;
expression SET;
expression N;
statement S;
@@

(
- if (strspn(BUF, SET) < N) S
+ if (klee_range(0, 2, "__pin_strspn") == 0) {
+   klee_assume((unsigned char)BUF[0] == 0xFFu);
+   if (strspn(BUF, SET) < N) S
+ }
|
- if (strspn(BUF, SET) <= N) S
+ if (klee_range(0, 2, "__pin_strspn") == 0) {
+   klee_assume((unsigned char)BUF[0] == 0xFFu);
+   if (strspn(BUF, SET) <= N) S
+ }
|
- if (strspn(BUF, SET) == 0) S
+ if (klee_range(0, 2, "__pin_strspn") == 0) {
+   klee_assume((unsigned char)BUF[0] == 0xFFu);
+   if (strspn(BUF, SET) == 0) S
+ }
|
- if (!strspn(BUF, SET)) S
+ if (klee_range(0, 2, "__pin_strspn") == 0) {
+   klee_assume((unsigned char)BUF[0] == 0xFFu);
+   if (!strspn(BUF, SET)) S
+ }
)

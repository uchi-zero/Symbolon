// pin-symbolic-bound-mem.cocci
//
// Rewrites memcpy / memmove / memset calls whose third argument is a
// symbolic length into a form that concretizes the length before the
// call. memcpy/memmove/memset with a symbolic byte count fork once per
// possible value of the count (up to the upper bound of the assumed
// range), which is the dominant explosion source in length-prefixed
// parsers.
//
// The transformation prepends `klee_assume(N == PIN_VALUE)` immediately
// before the call. After this, the call runs with a concrete length,
// producing exactly one path. Subsequent code that uses the same N
// variable also sees N == PIN_VALUE.
//
// PICKING PIN_VALUE: this rule defaults to 4, which satisfies the common
// 4-byte magic-number compare ("GIF8", "\x89PNG"+1, "PK\x03\x04", "ELF",
// "\xff\xd8\xff", etc.). For other downstream checks, edit the constant
// in each `klee_assume(N == 4)` line below to the smallest N that lets
// the predicate be satisfied. If you don't know — start by reading the
// downstream code that consumes the destination buffer. Picking too
// small a value silently kills coverage of the OK branch; picking too
// large a value is wasteful but not incorrect.
//
// IN SCOPE — patterns this cocci rewrites:
//
//   memcpy(dst, src, n);
//   memmove(dst, src, n);
//   memset(dst, c, n);
//
//   where n is a single variable identifier (not a literal, not an
//   expression). Whether n is actually symbolic at this call site is
//   the user's responsibility — the cocci injects unconditionally.
//
// OUT OF SCOPE — patterns NOT rewritten:
//
//   1. Length is a literal:
//        memcpy(dst, src, 16);
//      Reason: the `identifier` metavariable below excludes constants.
//      Manual remedy: not needed — concrete-length calls don't fork.
//
//   2. Length is a non-trivial expression:
//        memcpy(dst, src, header.len);
//        memcpy(dst, src, n + 4);
//      Reason: same — only simple identifiers match. The expression
//      could safely be pinned but the cocci is intentionally narrow to
//      avoid surprises.
//      Manual remedy: introduce a temporary `size_t k = expr;` then run
//      the cocci, or write the pin by hand.
//
//   3. The pinned value misses the downstream OK branch:
//        memcpy(dst, src, n);
//        if (dst[7] == 'X') { ... }   // needs n >= 8, but PIN_VALUE=4
//      Reason: the cocci's default pin value (4) is a guess. When the
//      downstream code needs more bytes than that, the OK branch is
//      LOST. The diff still applies cleanly; the loss is silent.
//      Manual remedy: edit each `klee_assume(N == 4)` line below to the
//      smallest value that satisfies the downstream predicate. Or, for
//      programs with multiple length-dependent branches, write a
//      hand-written klee_range over candidate values (see tests/
//      remedy-01.c for the indicator-pinned shape).
//
//   4. The pinned value conflicts with an existing constraint on N:
//        klee_assume(n >= 8);  // already constrained elsewhere
//        memcpy(dst, src, n);   // we inject klee_assume(n == 4)
//      Reason: the conjunction is unsatisfiable, so KLEE kills the
//      state. Worse than a no-op.
//      Manual remedy: change PIN_VALUE to match the existing range, or
//      remove the cocci's injection at this site.
//
// Usage:
//   spatch --sp-file pin-symbolic-bound-mem.cocci file.c            # preview
//   spatch --sp-file pin-symbolic-bound-mem.cocci --in-place file.c # apply

@ rewrite_memcpy @
expression DST;
expression SRC;
identifier N;
@@

+ klee_assume(N == 4);
  memcpy(DST, SRC, N);

@ rewrite_memmove @
expression DST;
expression SRC;
identifier N;
@@

+ klee_assume(N == 4);
  memmove(DST, SRC, N);

@ rewrite_memset @
expression DST;
expression C;
identifier N;
@@

+ klee_assume(N == 4);
  memset(DST, C, N);

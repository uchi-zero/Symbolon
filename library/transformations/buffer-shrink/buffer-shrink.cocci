// buffer-shrink.cocci
//
// DETECTION-ONLY rule for the buffer-shrinking pattern (Cadar, FSE
// 2015, pattern 6). Flags large symbolic buffer declarations that are
// likely to cause path explosion when iterated byte-by-byte.
//
// SYMPTOM: KLEE explores a per-byte branch in a loop over the buffer,
// producing 2^BUFSIZE paths. For BUFSIZE > ~20 this is intractable.
//
// REMEDY (semantics-altering, manual): pick a smaller BUFSIZE that
// still exercises the relevant algorithmic structure, accepting that
// bugs requiring specific large-input patterns won't be found.
//
// IN SCOPE — patterns this cocci flags:
//
//   1. Large stack-allocated array passed to klee_make_symbolic:
//        char buf[N]; klee_make_symbolic(buf, sizeof(buf), "buf");
//      where N is a literal >= 64 (rule of thumb: above ~64 a per-byte
//      branch loop becomes intractable).
//
//   2. Large macro-defined buffer:
//        #define BUFSIZE 1024
//        char buf[BUFSIZE]; klee_make_symbolic(...);
//      The cocci matches the array declaration; the macro size is
//      visible in the .c after preprocessing.
//
// OUT OF SCOPE — patterns NOT flagged:
//
//   1. Small buffers (< 64 bytes). These usually don't explode.
//
//   2. Symbolic file input via --sym-files. KLEE's POSIX harness sets
//      this up, not the user's source — different control point.
//
//   3. Heap-allocated buffers. Match-by-malloc is doable but each
//      malloc shape varies; we keep the rule narrow.
//
// MANUAL REMEDY PATTERN:
//
//   BEFORE:                             AFTER:
//     char buf[1024];                     char buf[16];
//     klee_make_symbolic(                 klee_make_symbolic(
//         buf, sizeof(buf), "buf");           buf, sizeof(buf), "buf");
//
// Just shrink the literal. The rest of the program (loop bound, indices)
// often picks up the change automatically via `sizeof(buf)`.
//
// COMMON PITFALLS:
//
//   - Shrinking too aggressively. If the program looks for a specific
//     pattern past byte K, BUFSIZE must be >= K. Read the loop bounds
//     and indices before picking the shrunk size.
//
//   - Forgetting that downstream code may have hardcoded the old size.
//     Look for the old literal (1024) elsewhere in the file.
//
// Usage:
//   spatch --sp-file buffer-shrink.cocci file.c

@ sym_arr @
type T;
identifier V;
expression SIZE;
expression NAME;
position p;
@@

T V[SIZE];
...
klee_make_symbolic@p(V, sizeof(V), NAME);

@script:python@
v << sym_arr.V;
sz << sym_arr.SIZE;
p << sym_arr.p;
@@
sz_str = str(sz)
try:
    n = int(sz_str)
    flag = (n >= 64)
    note = "size %d bytes" % n
except ValueError:
    # Non-literal size (macro, sizeof, expression) — flag for review.
    flag = True
    note = "size '%s' (non-literal — verify)" % sz_str
if flag:
    print("%s:%s:%s: symbolic buffer %s, %s — candidate for shrinking" %
          (p[0].file, p[0].line, p[0].column, v, note))

// pin-symbolic-array-index.cocci
//
// DETECTION-ONLY rule for the array-indexed-by-symbolic-value pattern
// from Perry et al., ISSTA 2017 ("Accelerating Array Constraints in
// Symbolic Execution"). This is an OBSTACLE 2 (unsolvable queries)
// pattern, not an OBSTACLE 1 (path explosion) pattern — symptoms differ:
//
//   - Low completed-path count (often 1-4)
//   - >95% wall time in solver
//   - High avg-constructs-per-query (60-200+)
//   - Deeply nested `Read` operations in *.kquery files
//   - At sufficient scale: 0 completed paths within budget
//
// The rule flags two shapes of array read against a symbolic index.
//
// IN SCOPE — patterns this cocci flags:
//
//   1. Predicate over a single array read:
//        if (ARR[k] OP v) { ... }
//      where ARR is a (file-scope) constant array, k is a symbolic
//      expression, OP is a comparison.
//
//   2. Nested array reads (the Figure 2 DFA pattern):
//        x = ARR2[ARR1[k]];
//        x = ARR2[i + ARR1[k]];
//        for (...) state = next[base[state] + cls];
//      where two or more array reads chain through a symbolic state.
//
// The rule does NOT rewrite — the rewrite needs to read the array's
// contents to compute the satisfying index set (for transformation 1)
// or to inline the table as branchless arithmetic (for transformation
// 2). Coccinelle templates can't do this; the rewrite is driven by a
// separate analysis (an LLM, a Python script that parses the array
// initializer, or a custom Clang pass).
//
// THREE REMEDY DIRECTIONS — all eliminate or shrink the symbolic-array-read
// query; pick based on whether the array is a fixed lookup table (A/B) or a
// dynamically-sized container whose dimension is a tunable constant (C).
//
// (A) Perry et al., ISSTA 2017 — INDEX-BASED / VALUE-BASED REWRITE.
//     Works for ANY constant table. Enumerate the indices that satisfy
//     the predicate (or that share each value) and emit them as integer
//     comparisons.
//
//   - tests/remedy-01.c — base64 character-class table. The predicate
//     `b64[k] >= 0` becomes a bitwise-OR'd disjunction of range tests:
//         (k == 43) | ((k >= 47) & (k <= 57)) | ...
//     The table has no closed form; we can only enumerate.
//
//   - tests/remedy-02.c — DFA. Both nested reads are inlined as
//     branchless arithmetic. Each transition `dfa_next[s][k]` becomes
//     a sum of `((s == X) & (k == Y)) * <target_state>` terms.
//     At LOOPS=80 this drops wall time from 120s (timeout, 0 paths)
//     to 0.36s (2 completed paths).
//
// (B) Cadar, FSE 2015 — REVERSE THE PRECOMPUTATION (closed-form
//     recovery). Applies only when the table is a manually-precomputed
//     function (often a polynomial, hash, or small math). Replace each
//     `tbl[k]` call site with the closed form, then delete the table.
//
//   - tests/remedy-03.c — cubes lookup. `cubes[k]` is replaced with
//     `k*k*k` and the 1000-entry table is removed. Wall time drops from
//     2.07s to 0.22s. Cadar's 2015 numbers are 50s → 0.2s on the same
//     example — modern KLEE/STP has closed some of that gap but the
//     direction holds.
//
// (C) SHRINK THE ALLOCATION DIMENSION (combine 0.4.0, r20). Applies when
//     the array is NOT a fixed lookup table but a dynamically-sized
//     *container* — a hash table, bucket array, or free-list — that is
//     `calloc(SIZE, slot)`'d from a tunable constant and then indexed by a
//     symbolic value (`table[hash(sym) % SIZE]`). KLEE forwards the WHOLE
//     container to the solver on the symbolic-index read, so the query
//     cost scales with SIZE — at a production size like 32767 that is a
//     multi-megabyte symbolic-array read ("Symbolic memory read will send
//     N bytes"). When the symbolic environment can only populate a handful
//     of live entries (combine's two ≤50-byte sym-files hold ≤4 records),
//     shrink SIZE to just above that live-entry ceiling. The index stays
//     symbolic and the lookup logic is byte-for-byte unchanged; only the
//     array KLEE hands the solver shrinks. Gate the shrink to the analysis
//     build so production/replay is unaffected:
//         #ifdef KLEE_BITCODE
//         #define SIZE 64
//         #else
//         #define SIZE 32768
//         #endif
//
//   - tests/obstacle-04.c / remedy-04.c — a symbolic-keyed hash table.
//     Shrinking SIZE 4096 → 64 drops solver time ~91x (27.9 s → 0.31 s).
//     In combine this took DEFAULT_HASHTABLE_SIZE 32767 → 64 and let all
//     four searchers DRAIN the menu in ~8 s (vs never draining in 2 h),
//     raising the worst searcher 1511 → 1693.
//
// Comparing the directions: (A) is mechanical once you can read the table;
// (B) requires *recognising* the function; (C) needs neither — it is a
// one-constant change, but applies ONLY to a resizeable container whose
// live-entry count is bounded by the (small) symbolic input, NOT to a
// fixed table whose every slot is semantically meaningful (shrinking those
// changes behaviour). (A) keeps the program semantically equivalent at the
// array read; (B) deletes the array entirely (fine for a precomputation,
// not if other code reads it); (C) keeps full semantics under the real
// build via the KLEE_BITCODE gate. An LLM driving the rewrite should prefer
// (C) when the array is a sized container and the symbolic input is small,
// (B) when a closed form is obvious (cube, square, hash, sin-table), and
// fall back to (A) otherwise.
//
// COMMON PITFALLS for direction (A):
//
//   - Using `||` / `&&` for the disjunction. This restores the path
//     explosion that the source-level transformation was supposed to
//     avoid (KLEE forks on each clause's truth value).
//
//   - Using a `switch` statement to encode the lookup. Same issue:
//     KLEE forks on each `case`. The remedy must be branchless.
//
//   - Forgetting the value-based grouping for repeated entries. The
//     paper's value-based transformation groups consecutive indices
//     with the same value into a single range, dramatically shrinking
//     the resulting formula. Without it, a 256-entry table with 10
//     distinct values still expands to 256 terms instead of ~10.
//
// Usage:
//   spatch --sp-file pin-symbolic-array-index.cocci file.c
//
// Output: prints each match's location. The matches are candidates
// for manual or LLM-driven rewrite.

virtual report

@ predicate_match @
identifier ARR;
expression K;
constant V;
position p;
@@

(
ARR@p[K] == V
|
ARR@p[K] != V
|
ARR@p[K] < V
|
ARR@p[K] <= V
|
ARR@p[K] > V
|
ARR@p[K] >= V
)

@script:python@
arr << predicate_match.ARR;
p   << predicate_match.p;
@@
print("%s:%s:%s: symbolic array index predicate: %s[...]" %
      (p[0].file, p[0].line, p[0].column, arr))

@ nested_match @
identifier ARR_OUTER;
identifier ARR_INNER;
expression K;
position p;
@@

ARR_OUTER@p[ \( ARR_INNER[K] \| <+... ARR_INNER[K] ...+> \) ]

@script:python@
outer << nested_match.ARR_OUTER;
inner << nested_match.ARR_INNER;
p     << nested_match.p;
@@
print("%s:%s:%s: nested symbolic array index: %s[...%s[...]...]" %
      (p[0].file, p[0].line, p[0].column, outer, inner))

// 2D array access where neither index is a constant — common DFA pattern
// (e.g. `dfa_next[state][cls]` from Figure 2 of Perry et al.).
@ twod_match @
identifier ARR;
expression I, J;
position p;
@@

ARR@p[I][J]

@script:python@
arr << twod_match.ARR;
p   << twod_match.p;
@@
print("%s:%s:%s: 2D symbolic array index: %s[...][...]" %
      (p[0].file, p[0].line, p[0].column, arr))

// reverse-strength-reduction.cocci
//
// DETECTION-ONLY rule for the strength-reduction pattern from Cadar,
// FSE 2015 ("Targeted Program Transformations for Symbolic Execution"),
// Figure 2.
//
// The obstacle is a loop accumulator `k = k + c` where `c` is symbolic
// (or, more generally, a value KLEE will treat symbolically). Under
// native execution this is a classic strength reduction — addition is
// cheaper than multiplication. Under symbolic execution it builds a
// deeply nested `Add(c, Add(c, Add(c, ...)))` expression at each
// iteration, which is far harder for the solver than `Mul(c, i)`.
//
// The remedy is induction variable substitution: replace the
// accumulator with the closed-form `c * i` where `i` is the loop
// counter. The solver then sees multiplication by a *concrete* loop
// index, which is linear arithmetic.
//
// In the test fixture (500-iteration loop): obstacle times out at 120s
// with 0 completed paths; remedy finishes in 6s with both paths.
//
// IN SCOPE — patterns this cocci flags:
//
//   1. In-loop additive update of an accumulator:
//        for (E1; E2; E3) { ...; ACC = ACC + INC; ...; }
//        for (E1; E2; E3) { ...; ACC += INC; ...; }
//      where ACC was initialised before the loop and INC is loop-
//      invariant. The flag is a candidate, not a confirmation —
//      legitimate accumulators (e.g. `sum += a[i]` where a[i] varies
//      per iteration) are not strength-reduction artefacts.
//
// OUT OF SCOPE — patterns NOT flagged:
//
//   1. Multi-step accumulator chains (`k = k + c; m = m + k;`).
//      The rewrite to closed form is non-trivial; needs case-by-case.
//
//   2. Accumulators with non-constant stride (`k = k + a[i];`).
//      Not strength reduction in the strict sense — the stride varies.
//
//   3. Negative-stride / decrement accumulators (`k = k - c;`).
//      Same fix applies (`k = -c * i + initial`) but cocci doesn't
//      enumerate the variant.
//
// MANUAL REMEDY PATTERN (see tests/remedy-01.c):
//
//   BEFORE:
//     int k = 0;
//     for (int i = 0; i < N; i++) {
//       y[i] = k;
//       k = k + c;
//     }
//
//   AFTER:
//     for (int i = 0; i < N; i++) {
//       y[i] = c * i;
//     }
//
// COMMON PITFALLS:
//
//   - Reverting the rewrite when running under a native-perf benchmark.
//     This transformation is DSE-friendly but native-unfriendly. Build
//     the symbolic harness from the remedy form; ship the obstacle
//     form (or whatever the compiler decides) for production.
//
//   - Mistaking a legitimate accumulator for strength reduction.
//     `sum += a[i]` is NOT a strength-reduced multiplication — the
//     stride is a[i], which is per-iteration symbolic, not invariant.
//
// Usage:
//   spatch --sp-file reverse-strength-reduction.cocci file.c

// We match the assignment shape only — the SmPL form for "inside a
// for-loop, anywhere" interacts badly with declarations in the loop
// header. The user (or an LLM driving the rewrite) should verify the
// match is inside a loop and that INC is loop-invariant before applying
// the substitution. A grep of the locations below against `for|while`
// in the same file is usually enough.

@ acc_assign @
identifier ACC;
expression INC;
position p;
@@

ACC@p = ACC + INC

@script:python@
acc << acc_assign.ACC;
p   << acc_assign.p;
@@
print("%s:%s:%s: accumulator update (candidate for strength-reduction reversal): %s = %s + ..." %
      (p[0].file, p[0].line, p[0].column, acc, acc))

@ acc_compound @
identifier ACC;
expression INC;
position p;
@@

ACC@p += INC

@script:python@
acc << acc_compound.ACC;
p   << acc_compound.p;
@@
print("%s:%s:%s: compound accumulator (candidate for strength-reduction reversal): %s += ..." %
      (p[0].file, p[0].line, p[0].column, acc))

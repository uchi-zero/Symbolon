// float-to-int.cocci
//
// DETECTION-ONLY rule for the floating-point pattern from Cadar, FSE
// 2015, pattern 5 ("Targeted Program Transformations for Symbolic
// Execution"). Flags symbolic float/double variables that KLEE will
// either reject or silently concretise.
//
// SYMPTOM in KLEE: "WARNING ONCE: silently concretizing (reason:
// floating point) expression ... to value 0". Once concretised, the
// symbolic input becomes a single concrete value (usually zero) and
// all branches that depended on the FP comparison resolve to one side
// only — coverage of the other side is silently lost.
//
// REMEDY (semantics-altering, manual): replace float/double with int
// (or fixed-point), narrow the symbolic range, rewrite arithmetic in
// integer-domain equivalents. See tests/remedy-01.c. The transformation
// trades FP-corner-case coverage (NaN, subnormal, overflow saturation)
// for ability-to-explore-at-all. For most application code this is a
// good trade; for FP-corner-case-sensitive code (financial / numerical
// libraries) re-test with a fuzzer instead.
//
// IN SCOPE — patterns this cocci flags:
//
//   1. klee_make_symbolic on float/double variable:
//        float x;  klee_make_symbolic(&x, sizeof(x), "x");
//        double x; klee_make_symbolic(&x, sizeof(x), "x");
//
//   2. klee_make_symbolic on float/double array:
//        double a[N]; klee_make_symbolic(a, sizeof(a), "a");
//
// OUT OF SCOPE — patterns NOT flagged:
//
//   1. Symbolic FP input arriving via fread / argv / stdin without an
//      explicit float type declaration. The cocci can't tell without
//      type-flow analysis.
//
//   2. FP operations on values that aren't directly symbolic but become
//      FP-dependent through derivation. KLEE will still concretise
//      these; the obstacle is the same but the cocci can't see it
//      structurally.
//
// COMMON PITFALLS for the manual remedy:
//
//   - Forgetting to constrain the integer range. `int x` symbolic has
//     range -2^31..2^31-1, which can overflow on `x*x` etc. Add
//     klee_assume(x > LB && x < UB) to keep operations within int
//     range.
//
//   - Translating the predicate naively. A check `x > 0.5` in the
//     original FP code needs scaling: `x_scaled > 50` if x is in 1/100
//     fixed-point. The constants must move with the type.
//
//   - Assuming the remedy is sound. It's not — FP-specific behaviours
//     are lost. Document the change in the test plan and re-test FP
//     corners separately (fuzzer, manual concrete tests).
//
// Usage:
//   spatch --sp-file float-to-int.cocci file.c

@ flt_var @
type T;
identifier V;
position p;
expression NAME;
@@

T@p V;
...
klee_make_symbolic(&V, sizeof(V), NAME);

@ flt_arr @
type T;
identifier V;
position p;
expression SZ, NAME;
@@

T@p V[...];
...
klee_make_symbolic(V, SZ, NAME);

@script:python@
t << flt_var.T;
v << flt_var.V;
p << flt_var.p;
@@
if str(t) in ("float", "double", "long double"):
    print("%s:%s:%s: symbolic floating-point variable: %s %s (KLEE will concretise)" %
          (p[0].file, p[0].line, p[0].column, t, v))

@script:python@
t << flt_arr.T;
v << flt_arr.V;
p << flt_arr.p;
@@
if str(t) in ("float", "double", "long double"):
    print("%s:%s:%s: symbolic floating-point array: %s %s[...] (KLEE will concretise)" %
          (p[0].file, p[0].line, p[0].column, t, v))

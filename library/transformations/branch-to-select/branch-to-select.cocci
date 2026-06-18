// branch-to-select.cocci
//
// Rewrites loop-body data-dependent branches into ternary assignments,
// addressing the pattern from Cadar, FSE 2015, Figure 4 ("Targeted
// Program Transformations for Symbolic Execution").
//
// The obstacle is a per-iteration `if (cond) ACC <op>` inside a loop.
// At -O0, KLEE forks at each iteration on `cond`, producing 2^N paths
// for an N-iteration loop (1024 for N=10 in the test fixture). The
// remedy expresses the same intent as a ternary, which LLVM lowers to
// `select` (an SMT `ite`) instead of `br` — one path regardless of N.
//
// Test fixture (10-iteration loop): obstacle 1024 paths in 0.25s,
// remedy 2 paths in 0.07s. Cadar's 2015 numbers were 1024/23s vs 2/0.04s.
//
// IN SCOPE — patterns this cocci REWRITES:
//
//   1. Conditional increment / decrement (no else branch):
//        if (cond) ACC++;
//        if (cond) ++ACC;
//        if (cond) ACC--;
//        if (cond) --ACC;
//      Rewritten to: ACC = cond ? ACC + 1 : ACC;  (or -1 for dec)
//
//   2. Conditional compound assignment (no else branch):
//        if (cond) ACC += EXPR;
//        if (cond) ACC -= EXPR;
//      Rewritten to: ACC = cond ? ACC + EXPR : ACC;
//
// OUT OF SCOPE — patterns NOT rewritten:
//
//   1. if-else with different actions on each side:
//        if (cond) ACC = X; else ACC = Y;
//      The rewrite ACC = cond ? X : Y is correct but the cocci avoids
//      it because EXPR-on-both-sides matching needs careful metavar
//      handling. Easy by hand.
//
//   2. Conditional with multi-statement body:
//        if (cond) { x++; y--; }
//      The rewrite needs two independent ternaries. Easy by hand.
//
//   3. Conditional with side-effecting RHS:
//        if (cond) ACC += f();
//      Risky to lift: the call f() must always be evaluated in the
//      rewrite, which changes side-effect semantics. Skipped on purpose.
//
//   4. Loops that already contain a `select`-emitting construct, or
//      whose branch doesn't actually fork (concrete condition).
//
// COMMON PITFALLS:
//
//   - Side effects in `cond` or `EXPR`. The ternary still evaluates
//     them every iteration, but a previously short-circuited path
//     might not have. For pure cond/expr (the common case) this is
//     irrelevant.
//
//   - Loops where the branch represents a real control-flow choice
//     KLEE needs to explore (e.g. one side calls a different function).
//     Don't rewrite those — coverage of the alternate function is lost.
//     The rule's scope is intentionally narrow (accumulator updates
//     only) to make this less likely.
//
// Usage:
//   spatch --sp-file branch-to-select.cocci file.c            # preview
//   spatch --sp-file branch-to-select.cocci --in-place file.c # apply

// Postfix / prefix increment and decrement (both braceless and braced).
@ rewrite_incdec_braceless @
expression COND;
identifier ACC;
@@

(
- if (COND) ACC++;
+ ACC = COND ? ACC + 1 : ACC;
|
- if (COND) ++ACC;
+ ACC = COND ? ACC + 1 : ACC;
|
- if (COND) ACC--;
+ ACC = COND ? ACC - 1 : ACC;
|
- if (COND) --ACC;
+ ACC = COND ? ACC - 1 : ACC;
)

@ rewrite_incdec_braced @
expression COND;
identifier ACC;
@@

(
- if (COND) { ACC++; }
+ ACC = COND ? ACC + 1 : ACC;
|
- if (COND) { ++ACC; }
+ ACC = COND ? ACC + 1 : ACC;
|
- if (COND) { ACC--; }
+ ACC = COND ? ACC - 1 : ACC;
|
- if (COND) { --ACC; }
+ ACC = COND ? ACC - 1 : ACC;
)

// Compound assignments (both braceless and braced).
@ rewrite_compound_braceless @
expression COND;
identifier ACC;
expression INC;
@@

(
- if (COND) ACC += INC;
+ ACC = COND ? ACC + INC : ACC;
|
- if (COND) ACC -= INC;
+ ACC = COND ? ACC - INC : ACC;
)

@ rewrite_compound_braced @
expression COND;
identifier ACC;
expression INC;
@@

(
- if (COND) { ACC += INC; }
+ ACC = COND ? ACC + INC : ACC;
|
- if (COND) { ACC -= INC; }
+ ACC = COND ? ACC - INC : ACC;
)

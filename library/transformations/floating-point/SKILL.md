---
name: floating-point
description: Use when a symbolic float/double makes KLEE emit 'WARNING ONCE: silently concretizing (reason: floating point) ... to value 0', collapsing every FP-dependent branch to one side and silently losing coverage of the other
---

# floating-point

**Category:** `unsolvable-queries` · **Kind:** Coccinelle detection-only (manual fix)

_Reference: Cadar, FSE 2015, pattern 5._

## Overview

Detection-only: flags symbolic `float`/`double`/`long double` variables and arrays passed to klee_make_symbolic. The manual remedy replaces FP with int (or fixed-point) and rewrites arithmetic in the integer domain.

## When to use

- klee_make_symbolic on a float/double scalar or array.
- Out of scope (cocci can't see structurally): symbolic FP arriving via fread/argv/stdin without an explicit float declaration, or values that become FP-dependent through derivation.

## How to apply

Run the cocci to list flagged declarations. Then by hand: replace the type with int/fixed-point, constrain the integer range (`klee_assume(x > LB && x < UB)` to avoid overflow on x*x etc.), and scale predicate constants with the type (`x > 0.5` -> `x_scaled > 50` for 1/100 fixed-point). Re-test FP corners (NaN, subnormal, overflow) separately with a fuzzer — the remedy is not sound for FP-corner-sensitive code.

## Evidence

Trades FP-corner-case coverage for ability to explore at all; good for most application code, wrong for numerical/financial libraries.
## Source files

Copied verbatim from `transformations/unsolvable-queries/floating-point/`:

- `float-to-int.cocci`
- `tests/`


---
name: strength-reduction
description: Use when a loop accumulator k = k + c with a loop-invariant (symbolic) stride c builds a deeply-nested Add(c, Add(c, ...)) expression that the solver chokes on — symptom is timeout with 0 completed paths
---

# strength-reduction

**Category:** `unsolvable-queries` · **Kind:** Coccinelle detection-only (manual fix)

_Reference: Cadar, FSE 2015, Figure 2._

## Overview

Detection-only: flags in-loop additive accumulator updates (`acc = acc + inc`, `acc += inc`). The manual remedy is induction-variable substitution — replace the accumulator with the closed form `c * i` (multiplication by a concrete loop index is linear arithmetic the solver handles easily).

## When to use

- An accumulator initialised before the loop is updated by a loop-invariant stride each iteration.
- Out of scope / NOT strength reduction: per-iteration-varying strides (`sum += a[i]`), multi-step chains (`k=k+c; m=m+k;`), negative strides (same fix, not enumerated).

## How to apply

Run the cocci to list candidate accumulator sites, then verify each is inside a loop with a loop-invariant stride (a grep of the locations against `for|while` usually suffices) before substituting the closed form by hand. Build the symbolic harness from the remedy form; ship the obstacle form for production (the rewrite is DSE-friendly but native-unfriendly).

## Evidence

Test fixture (500-iteration loop): obstacle times out at 120s with 0 completed paths; remedy finishes in 6s with both paths.
## Source files

Copied verbatim from `transformations/unsolvable-queries/strength-reduction/`:

- `reverse-strength-reduction.cocci`
- `tests/`


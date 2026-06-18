---
name: branch-to-select
description: Use when a loop body has a data-dependent conditional accumulator update (if (cond) acc++ / acc += expr) that KLEE forks on every iteration, producing 2^N paths
---

# branch-to-select

**Category:** `path-explosion` · **Kind:** Coccinelle rewrite (applies automatically)

_Reference: Cadar, FSE 2015 (Targeted Program Transformations for Symbolic Execution), pattern 4._

## Overview

Rewrite a per-iteration conditional accumulator update into a ternary, which LLVM lowers to `select` (an SMT `ite`) instead of a branch — one path regardless of iteration count.

## When to use

- A loop contains `if (cond) acc++;`, `if (cond) acc += expr;` (or `--`/`-=`) where the condition is symbolic.
- KLEE is fork-bound on the loop: path count grows like 2^N for an N-iteration loop.
- The branch is an accumulator update, not a real control-flow choice (e.g. one side calls a different function) — those must NOT be rewritten or coverage is lost.

## How to apply

Run the cocci to preview, then `--in-place` to apply. Scope is intentionally narrow (accumulator increment/decrement and compound +=/-=, braced or braceless); if-else with different actions, multi-statement bodies, and side-effecting RHS are out of scope and flagged in the patch header for hand-rewriting.

## Evidence

Test fixture (10-iteration loop): obstacle 1024 paths in 0.25s; remedy 2 paths in 0.07s. Cadar 2015: 1024/23s vs 2/0.04s.
## Source files

Copied verbatim from `transformations/path-explosion/branch-to-select/`:

- `branch-to-select.cocci`
- `tests/`


---
name: symbolic-array-index
description: Use when an array indexed by a symbolic value (arr[k], or nested arr2[arr1[k]] DFA-style) makes KLEE solver-bound: low completed-path count (1-4), >95% wall time in the solver, high avg-constructs-per-query (60-200+), deeply nested Read ops in .kquery files, or 0 completed paths at scale
---

# symbolic-array-index

**Category:** `unsolvable-queries` · **Kind:** Coccinelle detection-only (manual fix)

_Reference: Perry et al., ISSTA 2017 (Accelerating Array Constraints); Cadar, FSE 2015._

## Overview

Detection-only: flags predicates over a symbolic-indexed constant-array read, and nested array reads chaining through a symbolic state. Three remedy directions (the rewrite needs to read the array contents, so it's driven by an LLM/script/Clang pass, not Coccinelle).

## When to use

- Solver-bound (not fork-bound): few paths, near-total solver time, deeply nested Reads.
- `arr[k] OP v` with symbolic k over a file-scope constant array, or nested `arr2[arr1[k]]` / `next[base[state]+cls]`.

## How to apply

Pick a remedy: (A) Index/value-based rewrite [Perry 2017] — enumerate satisfying indices as a branchless bitwise-OR'd disjunction of range tests (NEVER ||/&&/switch — those re-introduce forks); works for any constant table. (B) Closed-form recovery [Cadar 2015] — when the table is a precomputed function (cube, square, hash, sin), replace `tbl[k]` with the formula and delete the table. (C) Shrink the allocation dimension — when it's a sized container (hash table/bucket array) `calloc(SIZE,...)`'d and indexed by `hash(sym)%SIZE`, shrink SIZE to just above the live-entry ceiling under `#ifdef KLEE_BITCODE` (production size in #else). Prefer C for sized containers with small symbolic input, B when a closed form is obvious, A otherwise.

## Evidence

Direction A DFA (LOOPS=80): 120s timeout/0 paths -> 0.36s/2 paths. Direction B cubes: 2.07s -> 0.22s. Direction C hash table: SIZE 4096->64 cut solver time ~91x (27.9s -> 0.31s); in 'combine', 32767->64 let all four searchers drain the menu in ~8s vs never in 2h.
## Source files

Copied verbatim from `transformations/unsolvable-queries/symbolic-array-index/`:

- `pin-symbolic-array-index.cocci`
- `tests/`


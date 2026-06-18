---
name: solver-hotspot-stub
description: Use when a single cleanup function (destructor, hash-table teardown, statistics-finaliser, log-flusher invoked from exit()/__cxa_finalize) dominates >=80% of total Qtime in klee-out/run.istats yet contributes nothing to source-line coverage, and the run is solver-bound (not fork-bound)
---

# solver-hotspot-stub

**Category:** `unsolvable-queries` · **Kind:** manual pattern (no .cocci)

_Reference: Symbolon transformation pattern._

## Overview

Replace the hotspot function's body with a no-op under `#ifdef KLEE_BITCODE`. KLEE then spends its budget on the rest of the program; the instrumented coverage build (without -DKLEE_BITCODE) keeps the original body, so ktest replay behavior is preserved.

## When to use

- One function/cluster accounts for the bulk of Qtime in run.istats (the README ships an awk one-liner to find it).
- It is off the coverage-critical path: cleanup/finalisation invoked after coverage-relevant work is done.
- Solver-bound (Qtime >> executor time, avg constructs/query >= 100), with bounded fork count — NOT a path-explosion stub.

## How to apply

Wrap the body in `#ifdef KLEE_BITCODE` / no-op (return a sensible default, `(void)`-cast args) / `#else` / original / `#endif`. ONLY safe when: (1) the return value is discarded/tolerated by all callers, (2) side-effects (frees, prints, mutations) aren't observed downstream, AND (3) it runs after coverage-relevant work. Verify by replaying a remedied ktest against the cov build (no -DKLEE_BITCODE) and confirming the lines you care about are still hit.

## Evidence

libtiff TIFFHashSetClearInternal: Qtime 3521s/3568s (97%) -> 0; paths 1268 -> 45104 (+3458%); lines 2002 -> 2410 (+20%). Toy: completed paths 50 -> 10, same 5 printf branches covered.
## Source files

Copied verbatim from `transformations/unsolvable-queries/solver-hotspot-stub/`:

- `README.md`
- `tests/`


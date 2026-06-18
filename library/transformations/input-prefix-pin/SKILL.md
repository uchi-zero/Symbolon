---
name: input-prefix-pin
description: Use when KLEE coverage plateaus at a parser's tokenisation/entry layer (parser file at 20-40%, the semantic rule/action engine file at 0%) and the run is fork-bound, not solver-bound
---

# input-prefix-pin

**Category:** `path-explosion` · **Kind:** manual pattern (no .cocci)

_Reference: Symbolon transformation pattern._

## Overview

Pin a structurally-valid prefix of the symbolic input to a known-valid shape via `klee_assume(buf[i] == ...)`, so KLEE drives straight into the deep parser branch in one path while the tail stays symbolic.

## When to use

- Lots of hits in tokenisation/line-reading code, zero hits in semantic-action code.
- State queue large with many partial states (fork-bound), not solver-bound.

## How to apply

After the symbolic input is loaded but BEFORE the parser runs, emit a sequence of `klee_assume(buf[i] == 'expected')` forming a valid prefix — long enough to reach deep branches, short enough to leave the tail symbolic. The prefix is program-specific domain knowledge (`"all:\n\techo ok\n"` for make, `"<?xml version="` for XML). For binary formats, pin the structural bytes (magic, class, table offsets/counts) and leave the semantic bytes (type/flags) symbolic.

## Evidence

Toy parser: 30 paths/1.03s without pin -> 2 paths/0.89s with pin (15x reduction), only the pinned run reaches the deep branch. readelf ELF-header toy: obstacle 705 tests (500 budget-bound) -> remedy 8 tests, all 8 segment-type branches saturated with zero wasted forks.
## Source files

Copied verbatim from `transformations/path-explosion/input-prefix-pin/`:

- `README.md`
- `tests/`


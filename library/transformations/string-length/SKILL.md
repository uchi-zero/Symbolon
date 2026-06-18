---
name: string-length
description: Use when an if-condition tests strlen/strnlen of a symbolic buffer (if (strlen(buf) < N) ...), forking once per byte position looking for the null terminator (up to buffer capacity + 1 paths)
---

# string-length

**Category:** `path-explosion` · **Kind:** Coccinelle rewrite (applies automatically)

_Reference: Symbolon path-explosion family (indicator-pin)._

## Overview

Pin the buffer length so the length check resolves in O(1) paths. The default pin makes the buffer empty (strlen == 0), driving the truthy branch of any predicate satisfied by zero-length input.

## When to use

- Predicate is satisfiable with len == 0: `< N`, `<= N`, `== 0`, `!strlen(...)`.
- Out of scope: predicates needing len > 0 (`> N`, `>= 1`, `== K`) — use klee_assume_strlen_one or hand-pin; result-assigned-to-variable; strcpy/strcat copy loops (pin the source's null by hand).

## How to apply

Run the cocci. It inserts `klee_assume_strlen_zero(buf)` (or use `klee_assume_strlen_one(buf, c)` for len==1) from klee_helpers.h, added if missing.

## Evidence

Pins the per-byte null-terminator fork to a single concrete length.
## Source files

Copied verbatim from `transformations/path-explosion/string-length/`:

- `klee_helpers.h`
- `pin-string-length.cocci`
- `tests/`


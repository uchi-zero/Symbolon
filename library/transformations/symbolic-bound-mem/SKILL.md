---
name: symbolic-bound-mem
description: Use when memcpy/memmove/memset is called with a symbolic byte count (memcpy(dst, src, n) where n is symbolic), forking once per possible value of the count — the dominant explosion source in length-prefixed parsers
---

# symbolic-bound-mem

**Category:** `path-explosion` · **Kind:** Coccinelle rewrite (applies automatically)

_Reference: Symbolon path-explosion family._

## Overview

Prepend `klee_assume(n == PIN_VALUE)` immediately before the call so it runs with a concrete length, producing exactly one path. Downstream code using the same n also sees the concrete value.

## When to use

- The length argument is a single variable identifier (not a literal, not an expression). The cocci injects unconditionally — whether n is actually symbolic at the site is the user's responsibility.
- Out of scope: literal lengths (don't fork), non-trivial expression lengths (introduce a temp first), and sites where the pin conflicts with an existing constraint on n (makes the state unsatisfiable).

## How to apply

Run the cocci. It defaults PIN_VALUE to 4 (satisfies common 4-byte magic compares: "GIF8", "PK\x03\x04", ...). Edit each `klee_assume(n == 4)` line to the smallest value that satisfies the downstream predicate — picking too small SILENTLY kills the OK branch; too large is wasteful but correct. For multiple length-dependent branches, hand-write a klee_range over candidate values.

## Evidence

Concretizes the count fork to a single path; the pin value is a guess that must be matched to downstream consumers of the destination buffer.
## Source files

Copied verbatim from `transformations/path-explosion/symbolic-bound-mem/`:

- `pin-symbolic-bound-mem.cocci`
- `tests/`


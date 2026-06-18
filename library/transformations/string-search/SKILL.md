---
name: string-search
description: Use when an if-condition searches a symbolic buffer in boolean context (if (strstr/strchr/strrchr/memchr/strpbrk(buf, target)) ...), forking once per candidate position plus per-byte forks
---

# string-search

**Category:** `path-explosion` · **Kind:** Coccinelle rewrite (applies automatically)

_Reference: Symbolon path-explosion family (indicator-pin)._

## Overview

Boolean-context use only needs SOME concrete input that satisfies the check, so pin a single position (the start of the buffer) to the target rather than enumerating all positions. O(1) paths.

## When to use

- FN in {strstr, strchr, strrchr, memchr, strpbrk} (and memmem with a literal needle) directly in an if-condition, tested as truthy / `!= NULL`.
- Out of scope: result assigned to a pointer then offset-tested or dereferenced past the match (pin would put match at buf[0] and break the offset predicate); non-literal memmem needle; inverted 'not found' predicates.

## How to apply

Run the cocci. It pins via `klee_assume_starts_with` (strstr), `klee_assume_starts_with_bytes` (memmem), `klee_assume_has_char` (strchr/memchr/strrchr), or `klee_assume_has_any` (strpbrk) from klee_helpers.h. Coverage-preserving for in-scope patterns; silently skips out-of-scope ones (cannot make them worse).

## Evidence

O(1) completed paths per call vs O(positions x bytes) forks.
## Source files

Copied verbatim from `transformations/path-explosion/string-search/`:

- `klee_helpers.h`
- `pin-string-search.cocci`
- `tests/`


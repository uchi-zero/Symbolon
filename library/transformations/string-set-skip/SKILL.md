---
name: string-set-skip
description: Use when an if-condition uses strcspn/strspn over a symbolic buffer against a literal character set (if (strcspn(buf, "set") < N) ...), forking O(buffer_size x set_size) per call
---

# string-set-skip

**Category:** `path-explosion` · **Kind:** Coccinelle rewrite (applies automatically)

_Reference: Symbolon path-explosion family (nasm hotspots)._

## Overview

Pin the first byte of the buffer so the span/complement-span function returns 0, satisfying the common surrounding check `if (FN(s, set) < N)` in O(1) paths.

## When to use

- strcspn/strspn with a string-literal set, inside an if-condition with a predicate satisfiable by a 0 return (`< N`, `<= N`, `== 0`, `!FN(...)`).
- Out of scope: result-assigned-to-variable; predicates needing a non-zero return (`> N`); the advancing-pointer idiom `line += strcspn(line, ...)`.

## How to apply

Run the cocci. For strcspn it pins buf[0] to set[0] (in-set, so the not-in-set scan stops at 0); for strspn it pins buf[0] to 0xFF (out-of-set for printable-ASCII sets — hand-pick a different value if the set contains 0xFF).

## Evidence

Found in nasm hotspots (nasm.c line-end, directiv.c name token, outbin.c whitespace skipping), each contributing O(buffer x set) forks per parser invocation.
## Source files

Copied verbatim from `transformations/path-explosion/string-set-skip/`:

- `pin-string-set-skip.cocci`
- `tests/`


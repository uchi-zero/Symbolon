---
name: buffer-shrink
description: Use when a large symbolic buffer (>= ~64 bytes) is iterated byte-by-byte, producing 2^BUFSIZE paths that make exploration intractable
---

# buffer-shrink

**Category:** `path-explosion` · **Kind:** Coccinelle detection-only (manual fix)

_Reference: Cadar, FSE 2015, pattern 6._

## Overview

Detection-only: flags large stack-allocated symbolic buffers (`char buf[N]; klee_make_symbolic(buf, sizeof(buf), ...)` with N >= 64). The manual remedy is to shrink the literal size while still exercising the relevant algorithmic structure.

## When to use

- A per-byte branch loop runs over a symbolic buffer and path count explodes (~2^BUFSIZE).
- The buffer is a literal-sized stack array fed to klee_make_symbolic; small (<64) buffers and --sym-files input are out of scope.

## How to apply

Run the cocci to list candidate declarations with their sizes. Then shrink the literal by hand to the smallest size that still exercises the algorithm — read the loop bounds and indices first (if the program inspects byte K, BUFSIZE must be >= K), and check the old literal isn't hardcoded elsewhere. `sizeof(buf)` usually propagates the new size automatically.

## Evidence

Semantics-altering: bugs requiring specific large-input patterns won't be found at the shrunk size.
## Source files

Copied verbatim from `transformations/path-explosion/buffer-shrink/`:

- `buffer-shrink.cocci`
- `tests/`


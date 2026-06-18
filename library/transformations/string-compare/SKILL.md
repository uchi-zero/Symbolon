---
name: string-compare
description: Use when an if-condition compares a symbolic buffer against a fixed string literal (if (!memcmp/strncmp/strcmp(buf, "lit", n)) ...), forking O(N) times — one early-exit path per byte position plus a full-match path
---

# string-compare

**Category:** `path-explosion` · **Kind:** Coccinelle rewrite (applies automatically)

_Reference: Symbolon path-explosion family (indicator-pin)._

## Overview

Introduce a pre-fork on an indicator variable; on the match branch, call a helper that adds one equality constraint per byte to the path constraint without branching. Net result: O(1) completed paths per call.

## When to use

- FN in {memcmp, strncmp, bcmp, strcmp, strcasecmp, strncasecmp} called directly inside an if-condition against a string literal.
- Out of scope: result assigned to a variable then tested, comparison against a non-literal, or inverted (!= 0) predicates.

## How to apply

Run the cocci (preview, then --in-place). It pins the match branch via `klee_assume_eq_bytes` (memcmp/strncmp/bcmp) or `klee_assume_eq_cstr` (strcmp, also pins the null terminator) and adds `#include "klee_helpers.h"` if missing — the shipped header must be on the include path at build time.

## Evidence

Helpers loop with concrete bounds, so KLEE unrolls them without forking — equivalent cost to a hand-unrolled klee_assume sequence.
## Source files

Copied verbatim from `transformations/path-explosion/string-compare/`:

- `klee_helpers.h`
- `pin-string-compare.cocci`
- `tests/`


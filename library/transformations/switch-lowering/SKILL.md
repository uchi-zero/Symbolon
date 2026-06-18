---
name: switch-lowering
description: Use when KLEE makes no progress on a switch with many cases and llvm-dis confirms a linear-chain lowering (where the default branch is the interesting case, as in most parsers)
---

# switch-lowering

**Category:** `path-explosion` · **Kind:** manual pattern (no .cocci)

_Reference: Cadar, FSE 2015, pattern 3._

## Overview

A switch can lower to a linear chain (path constraint x != 1 ∧ x != 2 ∧ ...), a balanced binary tree (a pair of range tests at depth log2 N), or a jump table. The chain shape buries deep cases; the remedy steers the lowering toward a balanced tree.

## When to use

- KLEE stuck on a many-case switch in a parser-like context.
- `llvm-dis program.bc | grep -A4 'switch '` confirms a linear-chain lowering.
- You can't change build flags OR flags didn't help.

## How to apply

Recommended action order (cheapest first): (1) `clang -O1 -Xclang -disable-llvm-passes` or `-O1 -fno-jump-tables`; (2) force `-mllvm -switch-range-to-icmp=true`; (3) ONLY if neither works, apply the source-level rewrite — restructure the switch as a balanced if-else tree (see tests/remedy-01.c), at the cost of verbose generated code. These are clang bitcode-emit flags, not KLEE flags.

## Evidence

Cadar Fig.3: divide-by-zero in the default branch of an 8-case switch — linear chain + BFS > 1 hour; binary tree + BFS 2.3s. (Modern clang at -O0 already lowers this fairly flat; the dramatic gap needed a specific clang/-fno-jump-tables/search-frontier combination.)
## Source files

Copied verbatim from `transformations/path-explosion/switch-lowering/`:

- `README.md`
- `tests/`


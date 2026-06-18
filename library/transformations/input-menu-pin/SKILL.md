---
name: input-menu-pin
description: Use when a parser/interpreter has a menu of mutually-exclusive sibling deep branches (a calculator's add/mul/div handlers, a parser's statement kinds, a protocol's message types) and no single pinned input reaches more than one — per-file coverage shows one feature's engine entered and its siblings at 0%
---

# input-menu-pin

**Category:** `path-explosion` · **Kind:** manual pattern (no .cocci)

_Reference: Symbolon transformation pattern (generalises input-prefix-pin)._

## Overview

Pin the symbolic input to one of a menu of known-valid, terminating inputs selected by a symbolic `klee_range`. KLEE forks once on the selector and explores every menu entry in a single run, covering all deep branches with one bitcode build and one manifest — never a cross-run profraw union.

## When to use

- input-prefix-pin works but only reaches one branch family; siblings stay at 0%.
- The deep branches are mutually exclusive in the input — you cannot pin one input that exercises all of them, and stacking individual pins is impossible (they pin the same bytes to contradictory values).
- Each menu entry, run alone, terminates (emits a ktest).

## How to apply

Write a `static const char *const menu[]` of complete, valid, terminating inputs, plus a `klee_range(0, NMENU, ...)` selector placed right after the symbolic input is read and before it is parsed. In each fork, klee_assume the input equals the chosen entry and concretize the buffer (buf[i]=entry[i]) so downstream is fork-free. Guard with `#ifdef KLEE_BITCODE`. CRITICAL for min-distance searchers (nurs:md2u): pad every entry to the full symbolic width so no symbolic tail is left, or md2u starves to 0 ktests. See README for the sed-style companion pin when the program and its data are separate symbolic objects.

## Evidence

Toy (6-handler dispatcher): obstacle 48 completed paths; remedy 12 (one per entry), all six handlers covered, 4x fewer paths. At scale on bc 1.08.2: a 60-entry menu took execute.c 0/578 -> 417/578 and number.c 24/1128 -> 902/1128 in one run.
## Source files

Copied verbatim from `transformations/path-explosion/input-menu-pin/`:

- `README.md`
- `tests/`


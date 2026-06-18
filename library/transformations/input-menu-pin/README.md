# input-menu-pin

A program-specific transformation pattern (not a generic cocci rule), and the
multi-feature generalization of [`input-prefix-pin`](../input-prefix-pin/).

`input-prefix-pin` pins the symbolic input to **one** known-valid shape, so a
run reaches **one** deep branch family. That is the right tool when a single
valid input already exercises the code you care about. But many programs have a
*menu* of sibling deep branches — a calculator's `add`/`mul`/`div`/… handlers, a
parser's statement kinds, a protocol's message types — and no single pinned
input reaches more than one of them.

The naive fix (pin input A, run; pin input B, run; …; merge the coverage) is
**forbidden as a headline result** by `TRANSFORMATION.md` "Rules: what counts as
a result": that is a cross-run `llvm-profdata merge`, not what one KLEE run can
reach. `input-menu-pin` is the sanctioned single-run equivalent.

## The idea

Pin the symbolic input to **one of a menu** of known-valid inputs, selected by a
**symbolic `klee_range`**. KLEE forks once on the selector and explores every
menu entry **in a single run** — each branch:

1. picks a concrete menu entry (concrete pointer in that forked state),
2. `klee_assume`s the symbolic input equals it (so the emitted ktest replays
   that exact input on the native binary), and
3. concretizes the buffer (`buf[i] = entry[i]`) so the downstream match/parse
   is fork-free and any solver-heavy work stays concrete.

```c
unsigned sel = (unsigned) klee_range(0, NMENU, "menu_sel");
const char *p = menu[0];
for (unsigned k = 1; k < NMENU; k++) if (sel == k) p = menu[k];  /* concrete in each fork */
for (unsigned i = 0; i < strlen(p) && i < n; i++) {
  klee_assume(buf[i] == p[i]);   /* ktest carries this input */
  buf[i] = p[i];                 /* fork-free, concrete downstream */
}
```

The result is **one bitcode build, one KLEE run, one `manifest.json`** whose
ktests cover all `NMENU` deep branches — reproducible by a single command, never
a profraw union across runs.

## When to apply

- `input-prefix-pin` works but only reaches one branch family; per-file coverage
  shows the program's execution engine entered for one feature and 0 % for its
  siblings.
- The deep branches are mutually exclusive in the input (you cannot pin one
  input that exercises all of them), and stacking the individual pins is
  impossible (they pin the same bytes to contradictory values).
- Each menu entry, run alone, terminates (emits a ktest) — i.e. you already have
  N working `input-prefix-pin` inputs and want their union *in one run*.

## What you write

A `static const char *const menu[]` of complete, valid, **terminating** inputs
(for a REPL/interpreter, each should end in whatever makes the state exit —
`quit`, EOF, etc.), plus the `klee_range` selector above, placed right after the
symbolic input is read and before it is parsed. Guard the whole thing with
`#ifdef KLEE_BITCODE` so the native coverage binary never sees `klee_range` /
`klee_assume`.

### Pin the FULL symbolic width — leave no symbolic tail (min-distance searchers)

When the symbolic input is a fixed-width buffer (e.g. `--sym-stdin N` /
`--sym-files 1 N`) and a menu entry is shorter than `N`, the loop above pins only
`strlen(entry)` bytes and leaves the remaining `N - strlen(entry)` bytes
**symbolic**. With breadth-first / coverage-new searchers that tail is usually
harmless, but a minimum-distance-to-uncovered searcher (`nurs:md2u`) will chase
the symbolic tail into deep uncovered code and **never finish a path** — the
state count explodes to the `--max-memory` cap with 0 completed paths and 0
ktests, so that searcher's coverage collapses to 0 (observed on expat: a menu
that gave the other three searchers +900 lines gave md2u 0).

Fix: **pad every menu entry to exactly `N` bytes** with a trailing filler that
the parser accepts as a no-op after the input already terminates (trailing
whitespace/epilog for XML, a comment or newline for a REPL, NUL padding if the
format ignores it). With the tail concrete, each menu branch is a bounded
concrete path family that every searcher finishes. The combined rule:
menu entries must be **complete, terminating, AND full-width** (no symbolic
bytes left). Verified on expat (`containers/programs/expat`,
`r4-doctype-menu-pin.patch`): 16 padded XML documents, all four searchers reach
an identical 3,092 lines with `stop=completed` (all paths exhausted); the same
16 documents UNpadded starve `nurs:md2u` to 0.

## Verification (tests/)

`tests/obstacle-01.c` is a dispatcher with six deep command handlers
(`add/mul/div/pow/mod/sqr`). `tests/remedy-01.c` adds a six-entry menu pinned by
`klee_range`.

```
clang -emit-llvm -c -g -O0 obstacle-01.c -o obstacle.bc
clang -emit-llvm -c -g -O0 remedy-01.c   -o remedy.bc
klee --posix-runtime --libc=uclibc --max-time=30s --output-dir=out-o obstacle.bc A --sym-files 1 15
klee --posix-runtime --libc=uclibc --max-time=30s --output-dir=out-r remedy.bc   A --sym-files 1 15
```

Obstacle: **48 completed paths** (fork-explosion on the symbolic dispatch, 8×
the handler count, the deep handlers buried among junk paths). Remedy: **12
completed paths**, one per menu entry (×2 for the file-size fork), each reaching
a distinct handler — all six deep branches covered, deterministically, in one
run with 4× fewer paths.

## Worked example: bc 1.08.2

`containers/programs/bc/transformations/unified-multipin.patch` is this pattern
at scale. bc's 40-byte symbolic stdin never assembles a complete, terminating
program under empc, so the execution engine is unreachable: the baseline covers
`execute.c` 0/578 and `number.c` 24/1128. A 60-entry menu of valid bc programs
(`1+1`, `for(...)…`, `define f…`, …) behind one `klee_range` takes a **single
run** to `execute.c` 417/578 and `number.c` 902/1128 — 3796/5339 lines (71 %).
See `containers/programs/bc/transformations/README.md`.

## When the menu pins the *program* but a separate input stays symbolic

bc's menu *is* the symbolic input (stdin = the bc program), so concretizing the
menu buffer (step 3) leaves nothing else symbolic. But some harnesses make the
**program** one symbolic object and the **data it processes** a *different*
symbolic object — e.g. sed's script is a symbolic `argv` while the file/stdin it
edits are separate symbolic bytes
(`containers/programs/sed/transformations/r1-script-menu-pin.patch`). Pinning
only the program is then a trap: the pinned program is now concrete, but if it
*matches the still-symbolic data* (a regex `s///` / `/re/`, a `y///` map, a
numeric comparison), the matcher runs the symbolic data through a solver-heavy
engine and KLEE either fork-explodes or hangs on an intractable query. (sed r1:
pinning only the script hung 3 of 4 searchers — every `s///` branch stalled in
gnulib `dfaexec`/`re_search` over the 16 symbolic input bytes; only the searcher
that happened to pick non-regex scripts survived.)

**Remedy: concretize the residual input too**, with a *companion* pin at the
point the program reads its data — a fixed template chosen to still exercise both
match and no-match paths (include a byte the menu's patterns match *and* bytes
they don't). Same three steps (assume → store), so the ktest still replays on the
native binary and the match stays fork-free. Rule of thumb: **after a menu-pin,
no symbolic object should reach a match/parse engine** — pin the program *and*
its input.

## Trade-offs

Semantics-altering, like `input-prefix-pin`: only the menu inputs (and their
error/recovery siblings) are explored; coverage of inputs you didn't enumerate
is forfeit. The menu is hand-written domain knowledge — KLEE varies *which* menu
entry, not the entries themselves. Adding entries grows coverage with
diminishing returns until the remaining gap is code unreachable from the harness
(see the bc residual analysis).

When a companion input-concretization is used (above), it narrows the *data*
coverage to the chosen template — a second semantics-altering pin. Pick a
template that still spans the patterns in your menu (sed r1 uses `"ab.XcA1z"`:
an `a` for the `/a/`-family matches, plus non-`a` bytes for `[^a]` / anchors /
no-match paths).

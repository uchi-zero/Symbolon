# input-prefix-pin

A program-specific transformation pattern, not a generic cocci rule.
When the symbolic input to a parser is too unconstrained — every byte
free, every dispatch branch reachable — KLEE burns budget on the
lexical / tokenisation layer and never reaches the program's semantic
core (the rule engine, the DOM tree, the build graph, etc).

The remedy is to pin a prefix of the symbolic input to a known-valid
shape, so KLEE goes straight to the deep parser branch in one path.

## When to apply

Symptoms:

- KLEE coverage plateaus at the **parser entry** of a program (lots of
  hits in the tokenisation / line-reading code, zero hits in the
  semantic-action code).
- Per-file coverage shows the file containing the parser at 20-40%
  but the file containing the rule/dependency/action engine at 0%.
- KLEE is running fork-bound (state queue large, many partial states)
  rather than solver-bound.

## What you write

For each parsable input the harness feeds to the program, write a
sequence of `klee_assume(buf[i] == 'expected_byte');` calls AFTER the
symbolic input has been loaded but BEFORE the parser is invoked. The
pin sequence should be a structurally-valid prefix of the input
language — long enough to drive the parser into its deep branches,
short enough to leave the tail symbolic for KLEE to vary.

### Example: GNU make harness

After `eval_makefile()` reads the Makefile bytes into a buffer, pin
the first ~14 bytes to `"all:\n\techo ok\n"`:

```c
klee_assume(buf[0] == 'a');
klee_assume(buf[1] == 'l');
klee_assume(buf[2] == 'l');
klee_assume(buf[3] == ':');
klee_assume(buf[4] == '\n');
klee_assume(buf[5] == '\t');
klee_assume(buf[6] == 'e');
klee_assume(buf[7] == 'c');
klee_assume(buf[8] == 'h');
klee_assume(buf[9] == 'o');
klee_assume(buf[10] == ' ');
klee_assume(buf[11] == 'o');
klee_assume(buf[12] == 'k');
klee_assume(buf[13] == '\n');
```

The remaining 26 bytes (assuming `--sym-files 1 40`) are still
symbolic. KLEE explores variations of the recipe / rule beyond byte 14
while the rule-engine entry is reached deterministically.

### Why this isn't a generic cocci

The pin contents — `"all:\n\techo ok\n"` for make, `"<?xml version="` for
an XML parser, `"GET / HTTP/1.1\r\n"` for an HTTP parser — are
program-specific. A cocci can detect the call site to pin (typically
right after `klee_make_symbolic` or `fread` from a symbolic source)
but cannot synthesise the prefix. Treat this as a manual transformation
driven by a developer or LLM with domain knowledge of the input
format.

## Verification (tests/)

`tests/obstacle-01.c` is a recursive-descent toy parser that checks
each byte of input against `"all:\n\techo ok\n"`. KLEE without the pin
explores 30 paths in 1.03s; KLEE with the pin (`tests/remedy-01.c`)
explores 2 paths in 0.89s — and only the pinned run reaches the
truthy branch (`printf("OK\n");`).

15× path-count reduction. Same pattern at scale on make's parser
should drive KLEE from "stuck at 14% line coverage" to "reaches the
rule / job / function modules at non-zero coverage."

### Binary-format example: the ELF header gate (readelf)

`tests/obstacle-02.c` / `tests/remedy-02.c` apply the same idea to a
*binary* input. The toy models readelf's `get_file_header`: a 4-byte
magic, the `EI_CLASS`/`EI_DATA`/`EI_VERSION` dispatch bytes, and the
structural locator fields `e_phoff` (program-header table offset) and
`e_phnum` (entry count), followed by a per-segment `p_type` switch
(`dispatch_segment`).

The obstacle leaves the whole header symbolic. KLEE solves the magic on
its own, but the symbolic `e_phoff`/`e_phnum` make the table bounds-check
and the symbolic-address read `buf[e_phoff + i*32]` fork over the entire
offset/count domain. In a 30 s budget (256-byte buffer) KLEE is
force-killed mid-exploration:

| | completed paths | partially completed (budget-bound) | tests |
|---|---|---|---|
| obstacle-02 | 205 | **500** | 705 |
| remedy-02   | 8   | **0**   | 8   |

The remedy pins the structural skeleton — magic, class/data/version, and
the table layout (`e_phoff = 32`, `e_phnum = 1`) — while leaving the
*semantic* word `p_type` symbolic. KLEE drives one state straight into
`dispatch_segment` and saturates all 8 segment-type branches with zero
wasted forks.

The lesson for binary formats: pin the bytes the parser uses to decide
*how to walk the input* (magic, class, table offsets/counts), and leave
the bytes it uses to decide *what each element means* (segment/section
type, machine, flags) symbolic. The structural pin removes the
offset/count fork explosion; the symbolic semantic tail is what KLEE
should spend its budget on. Applied to readelf itself this becomes
`containers/programs/readelf/transformations/elf-header-pin.patch`.

## Trade-offs

Semantics-altering. Coverage of the parser's error-handling paths
(invalid tokens, malformed rules, EOF mid-rule) is forfeit. For
test-generation targeting the SEMANTIC core, this is the right
trade — error paths are usually exhausted by static analysis or
fuzzing anyway.

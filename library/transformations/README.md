# Transformation Skills

14 hand-authored KLEE transformation skills, generated from `transformations/` by `scripts/gen_transformation_skills.py`. Each `<name>/SKILL.md` carries the trigger, remedy, and evidence; the transformation's real `.cocci` / `README.md` / `tests/` / `klee_helpers.h` are copied alongside as the source of truth.

To regenerate: `uv run scripts/gen_transformation_skills.py --clean`

## path-explosion

- [`branch-to-select`](branch-to-select/SKILL.md) — Use when a loop body has a data-dependent conditional accumulator update (if (cond) acc++ / acc += expr) that KLEE forks on every iteration, producing 2^N paths
- [`buffer-shrink`](buffer-shrink/SKILL.md) — Use when a large symbolic buffer (>= ~64 bytes) is iterated byte-by-byte, producing 2^BUFSIZE paths that make exploration intractable
- [`input-menu-pin`](input-menu-pin/SKILL.md) — Use when a parser/interpreter has a menu of mutually-exclusive sibling deep branches (a calculator's add/mul/div handlers, a parser's statement kinds, a protocol's message types) and no single pinned input reaches more than one — per-file coverage shows one feature's engine entered and its siblings at 0%
- [`input-prefix-pin`](input-prefix-pin/SKILL.md) — Use when KLEE coverage plateaus at a parser's tokenisation/entry layer (parser file at 20-40%, the semantic rule/action engine file at 0%) and the run is fork-bound, not solver-bound
- [`string-compare`](string-compare/SKILL.md) — Use when an if-condition compares a symbolic buffer against a fixed string literal (if (!memcmp/strncmp/strcmp(buf, "lit", n)) ...), forking O(N) times — one early-exit path per byte position plus a full-match path
- [`string-length`](string-length/SKILL.md) — Use when an if-condition tests strlen/strnlen of a symbolic buffer (if (strlen(buf) < N) ...), forking once per byte position looking for the null terminator (up to buffer capacity + 1 paths)
- [`string-search`](string-search/SKILL.md) — Use when an if-condition searches a symbolic buffer in boolean context (if (strstr/strchr/strrchr/memchr/strpbrk(buf, target)) ...), forking once per candidate position plus per-byte forks
- [`string-set-skip`](string-set-skip/SKILL.md) — Use when an if-condition uses strcspn/strspn over a symbolic buffer against a literal character set (if (strcspn(buf, "set") < N) ...), forking O(buffer_size x set_size) per call
- [`switch-lowering`](switch-lowering/SKILL.md) — Use when KLEE makes no progress on a switch with many cases and llvm-dis confirms a linear-chain lowering (where the default branch is the interesting case, as in most parsers)
- [`symbolic-bound-mem`](symbolic-bound-mem/SKILL.md) — Use when memcpy/memmove/memset is called with a symbolic byte count (memcpy(dst, src, n) where n is symbolic), forking once per possible value of the count — the dominant explosion source in length-prefixed parsers

## unsolvable-queries

- [`floating-point`](floating-point/SKILL.md) — Use when a symbolic float/double makes KLEE emit 'WARNING ONCE: silently concretizing (reason: floating point) ... to value 0', collapsing every FP-dependent branch to one side and silently losing coverage of the other
- [`solver-hotspot-stub`](solver-hotspot-stub/SKILL.md) — Use when a single cleanup function (destructor, hash-table teardown, statistics-finaliser, log-flusher invoked from exit()/__cxa_finalize) dominates >=80% of total Qtime in klee-out/run.istats yet contributes nothing to source-line coverage, and the run is solver-bound (not fork-bound)
- [`strength-reduction`](strength-reduction/SKILL.md) — Use when a loop accumulator k = k + c with a loop-invariant (symbolic) stride c builds a deeply-nested Add(c, Add(c, ...)) expression that the solver chokes on — symptom is timeout with 0 completed paths
- [`symbolic-array-index`](symbolic-array-index/SKILL.md) — Use when an array indexed by a symbolic value (arr[k], or nested arr2[arr1[k]] DFA-style) makes KLEE solver-bound: low completed-path count (1-4), >95% wall time in the solver, high avg-constructs-per-query (60-200+), deeply nested Read ops in .kquery files, or 0 completed paths at scale

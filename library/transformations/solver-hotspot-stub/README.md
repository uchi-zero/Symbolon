# solver-hotspot-stub

A program-specific transformation pattern, not a generic cocci rule.
When a single function dominates KLEE's solver budget — typically a
resource-cleanup routine called at every state's exit — and that
function's body contributes nothing to source-line coverage, the
remedy is to **replace its body with a no-op under
`#ifdef KLEE_BITCODE`**.

The instrumented coverage build (without `-DKLEE_BITCODE`) still has
the original function, so the cov binary's behavior on ktest replay is
preserved. KLEE's exploration spends its budget on the rest of the
program instead of on cleanup paths.

## When to apply

Symptoms:

- A single function (or a small cluster) accounts for ≥80 % of total
  `Qtime` in `klee-out/run.istats`:

  ```bash
  awk 'BEGIN{qi=0}/^events:/{for(i=2;i<=NF;i++)if($i=="Qtime")qi=i-1;next}
       /^fl=/{fl=substr($0,4);next}
       /^fn=/{fn=substr($0,4);skipnext=0;next}
       /^cfn=/{skipnext=1;next}
       /^cfl=|^calls=/{next}
       /^[0-9]/{if(skipnext){skipnext=0;next}if(qi>0)q[fl"|"fn]+=$(qi+2)}
       END{for(k in q)if(q[k]>0)print q[k]"|"k}' run.istats \
    | awk -F'|' '{printf "%10.1fs  %s  %s\n",$1/1e6,$2,$3}' \
    | sort -rn | head
  ```

- That function is **not on the coverage-critical path**: it's a
  destructor, hash-table teardown, statistics-finaliser, log-flusher,
  or similar cleanup invoked from `exit()` or `__cxa_finalize`.

- The engine is **solver-bound** (Qtime ≫ executor time per
  `run.stats`, average constructs/query ≥ 100), not fork-bound.

- Total queries scale with `states × work_per_state` because the
  hotspot runs for every surviving state.

## What you write

For each identified solver-hotspot function `F`, wrap its body with
`#ifdef KLEE_BITCODE`/`#else`/`#endif`:

```c
ReturnType F(Args... args) {
#ifdef KLEE_BITCODE
    // Stub: F is the dominant solver-time sink at cleanup; its return
    // value is discarded and its side-effects are coverage-irrelevant.
    // Replacing with a no-op saves N×states solver queries.
    (void)arg1; (void)arg2; /* ... */
    return /*sensible default*/ 0;
#else
    /* original body */
#endif
}
```

**Critical**: this is *only* safe when

1. F's return value is genuinely discarded by every caller (or callers
   are tolerant of a fixed return), AND
2. F's side-effects (frees, prints, state mutations) are not observed by
   downstream coverage-relevant code, AND
3. F is called *after* the program's coverage-relevant work has
   completed (cleanup, finalisation), so cutting F doesn't truncate
   exploration.

If any of the three is false, the stub will silently change observable
behavior. Verify by running the cov build (`-fprofile-instr-generate
-fcoverage-mapping`, no `-DKLEE_BITCODE`) against a ktest from the
remedied KLEE run; the replay should still hit all the lines you care
about.

## Worked example: libtiff TIFFHashSetClearInternal

In libtiff 4.7.1's `tif_hash_set.c`, `TIFFHashSetClearInternal` walks
a per-bucket linked list, calling `free` (and a user-provided free
function) on each entry. The function is called from `TIFFCleanup`
which runs from `TIFFClose` at the end of `tiffinfo` processing.

Under KLEE on `tiffinfo`'s symbolic-input harness, this teardown
dominated Qtime at **3 521 s out of 3 568 s (97 %)** because each
`while (cur)` test forks on a symbolic pointer derived from a
symbolic IFD offset. The teardown does no work that matters to
coverage — every coverage-relevant byte was already read by the
parser by the time `TIFFCleanup` runs.

The `r3-hashset-stub.patch` in
`containers/programs/libtiff/transformations/` replaces the loop body
with `return;` under `#ifdef KLEE_BITCODE`. Result on a 1-h KLEE
run, measured by replaying ktests against the unmodified
`program.cov`:

| metric | baseline (v2) | + r3 stub | delta |
|---|---|---|---|
| `TIFFHashSetClearInternal` Qtime | 3 521 s | 0 s | -100 % |
| total queries | 13 829 | 103 263 | **+646 %** |
| paths explored | 1 268 | 45 104 | **+3 458 %** |
| lines covered | 2 002 | 2 410 | +20 % |

The +408 lines come not from anything new in the hash-set code — that
file is at the same coverage either way — but from KLEE having
budget left over to explore deeper parser paths.

## Distinguishing this pattern from path-explosion stubs

If the dominant cost is in **fork count**, not Qtime, this isn't the
right pattern. Use `path-explosion/symbolic-bound-mem`,
`path-explosion/string-search`, etc. The solver-hotspot-stub pattern
applies when:

- Fork count is bounded (often 1 fork per cleanup iteration), but
- The per-query SMT cost is high because the path constraint has
  grown by the time cleanup runs, and
- Each surviving state pays the cost.

The signature is a function with modest fork count but large `Qtime`
in `run.istats`.

## Verification (tests/)

The toy pair under `tests/`:

- `obstacle-01.c` — five-branch `printf` paths followed by a
  symbolic-bound `cleanup_hotspot(n, pool)` whose return value is
  discarded.
- `remedy-01.c` — same program with `cleanup_hotspot`'s body wrapped
  in `#ifdef KLEE_BITCODE` / no-op stub.

Measured on a 10 s KLEE run (`klee --max-time=10s --solver-backend=z3`):

| metric | obstacle-01 | remedy-01 |
|---|---|---|
| total instructions | 1 087 | 157 |
| completed paths | **50** | **10** |
| generated tests | 50 | 10 |
| reaches all 5 printf branches | yes | yes |

The remedy reaches the same five printf coverage branches but uses
5× fewer paths because the symbolic-bound cleanup loop no longer
multiplies path count.

Reproduce:

```bash
cd transformations/unsolvable-queries/solver-hotspot-stub/tests
KLEE_INC=$(dirname "$(realpath "$(which klee)")")/../include
clang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone \
      -isystem "$KLEE_INC" obstacle-01.c -o obstacle-01.bc
clang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone \
      -isystem "$KLEE_INC" -DKLEE_BITCODE remedy-01.c -o remedy-01.bc
klee --max-time=10s --output-dir=out-obstacle obstacle-01.bc
klee --max-time=10s --output-dir=out-remedy   remedy-01.bc
grep -E "completed paths|generated tests|total instructions" \
   out-obstacle/info out-remedy/info
```

## Trade-offs

- **Semantics-altering** at the KLEE-build level: KLEE no longer explores
  what would happen if the cleanup function did its real work. For
  cleanup that's coverage-irrelevant (the usual case), this is exactly
  the point. For cleanup that *does* affect downstream code — e.g. a
  finaliser that writes a checksum that another function then reads —
  do not stub it.
- **Cov-binary preservation**: the original function body lives in the
  `#else` branch, so the coverage oracle compiled without `-DKLEE_BITCODE`
  runs the real cleanup. Ktest replay against `program.cov` exercises
  it normally with the concrete input KLEE chose.
- **Identification cost**: the static passes (`pass/fork-finder`,
  `pass/state-killer-finder`) don't flag solver-hotspots — those are
  visible only at runtime in `run.istats`. The first KLEE run on a new
  program is the diagnostic step.

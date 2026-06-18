# switch-lowering

Pattern 3 from Cadar, FSE 2015 ("Targeted Program Transformations for
Symbolic Execution"). This directory has no `.cocci` — the obstacle is
not a source-level rewrite, it's a clang/LLVM lowering choice. The
fixtures here document the trade-off so an LLM (or a human) can decide
whether to apply a manual source-level rewrite, change clang flags, or
change KLEE's searcher.

## The obstacle

A `switch` statement with many cases can be lowered into different IR
shapes:

1. **Linear chain** — `if (x==1) ... else if (x==2) ... else if (x==3) ...`
   At depth N the path constraint is `x != 1 ∧ x != 2 ∧ ... ∧ x != N`.
   Each case lives at successive depth in the search tree.

2. **Balanced binary tree** — `if (x <= mid) { if (x <= mid2) ... }`
   At depth log₂(N) the path constraint is a pair of range tests.

3. **Jump table** — clang at `-O1+` for dense case sets, emits a
   computed-goto-style table. KLEE follows the table indirection
   symbolically (one fork per possible target).

Cadar's Figure 3 reports the divide-by-zero bug in the `default` branch
of an 8-case switch reaches as follows:

| lowering         | searcher | time to bug |
|------------------|----------|-------------|
| linear chain     | BFS      | > 1 hour    |
| binary tree      | BFS      | 2.3 s       |

## Build flags that influence the lowering

These are passed to **clang at bitcode-emit time**, not to KLEE:

- `-O0 -fno-jump-tables` — forces linear chain. Worst for DSE.
- `-O1 -fno-jump-tables` — clang may emit binary tree.
- `-O1` (default) — clang picks jump table when cases are dense, else
  binary tree. Usually OK for DSE.
- `-mllvm -switch-range-to-icmp=true` — forces conversion of dense
  ranges to icmp-based binary tree even at higher -O.

Verify with `llvm-dis program.bc | grep -A4 'switch '`. If you see a
`switch i32 ... [` followed by many `i32 N, label %case_N` pairs, the
lowering is a jump-table-style multi-target instruction. KLEE handles
this by forking once per case (similar to linear chain, but flatter
control flow).

## The source-level remedy (only when clang flags are not available)

`tests/obstacle-01.c` and `tests/remedy-01.c` show the source
transformation: rewrite the `switch` as a balanced if-else tree. This
guarantees the desired lowering regardless of clang version or `-O`
level, at the cost of verbose, mechanically-generated code.

In our environment both versions hit the divide-by-zero bug within the
60s budget — modern clang at `-O0 -disable-O0-optnone` already lowers
this 8-case switch in a relatively flat shape. The dramatic 1-hour-vs-
2.3s gap in Cadar 2015 required a specific combination of clang
version, `-fno-jump-tables`, and a deeply-nested search-frontier that
favoured the linear chain.

## When to apply the source-level rewrite

Apply only when:

1. You can't change build flags (e.g. you're working from a binary you
   don't recompile).
2. You're observing KLEE making no progress on a switch with many
   cases, and `llvm-dis` confirms a linear-chain lowering.
3. The default branch is the interesting case (Cadar's setup — most
   parsers are like this).

Otherwise, change clang flags or switch KLEE's searcher first; those
are cheaper.

## What to put in the LLM prompt

The fixtures here show:
- The source-level transformation pattern (switch → balanced if-else)
- The trade-off (verbose source vs. control over lowering)
- The flag-level alternatives (often preferable)

When the LLM sees a large switch statement in a parser-like context
under KLEE, the recommended action order is:

1. Try `clang -O1 -Xclang -disable-llvm-passes ...` first (smallest change).
2. If unavailable, force `-mllvm -switch-range-to-icmp=true`.
3. Only if neither works, apply the source-level binary-tree rewrite
   in `tests/remedy-01.c`.

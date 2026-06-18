# Symbolon: Learning Transformations for Symbolic Execution

Symbolon raises the code coverage a symbolic-execution engine (KLEE) reaches on
a C program by **transforming the program's source** so it becomes more amenable
to symbolic execution — then measuring the coverage delta in a reproducible
harness. It pairs a diagnosis toolkit (static analyzers + runtime traces that
identify *why* a run is stuck) with a corpus of vetted source-level
transformations and an agent loop that applies them.

## Repository layout

```
.
├── flake.nix          # Nix flake: dev shell + per-program KLEE eval images
├── klee/              # KLEE submodule (feat/symbolon branch)
├── pass/              # LLVM 16 static analyzers — diagnose the obstacles
├── containers/        # KLEE evaluation harness — builds & runs per-program eval images
├── library/           # Agent Skills corpus — one SKILL.md per transformation
├── transformations/   # per-program transformation patches (one .patch per program)
└── scripts/           # host-side drivers (searcher sweep, metrics, watchers)
```

The pieces compose along one pipeline: **`pass/` diagnoses** what is blocking a
run, **`library/` supplies** the transformation that addresses it, **`containers/`
measures** the coverage a single KLEE run achieves before and after, and
**`transformations/`** records the best patch found per program.

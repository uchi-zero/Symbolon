#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EVAL_SYMBOLON="$SCRIPT_DIR/../eval-symbolon"
STUB_AGENT="$SCRIPT_DIR/fixtures/stub-agent/main.sh"
# shellcheck source=_klee-shim.sh
. "$SCRIPT_DIR/_klee-shim.sh"
# Inherited by eval-symbolon → eval-klee → klee child.
LD_PRELOAD="$(klee_shim_libpthread)"
export LD_PRELOAD

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# Lay out a baked program dir + a source tree the agent will transform.
PROG="$WORK/program"
mkdir -p "$PROG/src"
cat >"$PROG/src/hello.c" <<'EOF'
#include <stdio.h>
int main(int argc, char **argv) {
  if (argc > 1 && argv[1][0] == 'y') return 0;
  return 1;
}
EOF
cat >"$PROG/src/Makefile" <<'EOF'
CC ?= cc
CFLAGS ?= -O0 -g
hello: hello.c
	$(CC) $(CFLAGS) hello.c -o hello
EOF

# Build instrumented oracle and baseline bc.
clang -O0 -g -fprofile-instr-generate -fcoverage-mapping \
  "$PROG/src/hello.c" -o "$PROG/program.cov"
gclang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone \
  "$PROG/src/hello.c" -o "$PROG/program.bc"

cat >"$PROG/program.json" <<'EOF'
{
  "name": "hello",
  "version": "0.1.0",
  "with-libcxx": false,
  "sym-env": ["--sym-args", "0", "1", "1"]
}
EOF

# Run with budget that allows baseline + at least one attempt.
SYMBOLON_PROGRAM_DIR="$PROG" \
  "$EVAL_SYMBOLON" \
  --time-budget 90s \
  --max-attempts 2 \
  --search dfs \
  --max-time-per-attempt 10s \
  --cov-sample-every 2s \
  --final-max-time 10s \
  --agent-cmd "$STUB_AGENT" \
  --out "$WORK/out"

# Baseline + at least one attempt directory
test -d "$WORK/out/attempts/000-baseline"
test -s "$WORK/out/attempts/000-baseline/manifest.json"

mapfile -t attempts < <(find "$WORK/out/attempts" -mindepth 1 -maxdepth 1 -type d -name '[0-9]*' | sort)
((${#attempts[@]} >= 2)) # baseline + agent attempt(s)

# Each attempt past baseline has the agent rationale + a klee manifest
test -s "${attempts[1]}/agent-rationale.md"
test -s "${attempts[1]}/manifest.json"

# Top-level summary records best attempt and trajectory
test -s "$WORK/out/summary.json"
grep -q '"best"' "$WORK/out/summary.json"
grep -q '"trajectory"' "$WORK/out/summary.json"
grep -q '"agent"' "$WORK/out/summary.json"

# Baseline attempt has a trajectory CSV
test -s "$WORK/out/attempts/000-baseline/coverage-trajectory.csv"
[[ "$(head -1 "$WORK/out/attempts/000-baseline/coverage-trajectory.csv")" == "t_seconds,functions_covered,instantiations_covered,lines_covered,regions_covered,branches_covered,ktests_so_far" ]]

# Non-baseline agent attempts MUST NOT have a trajectory CSV
for d in "$WORK/out/attempts"/[0-9]*; do
  name="$(basename "$d")"
  if [[ $name != "000-baseline" ]]; then
    if [[ -e "$d/coverage-trajectory.csv" ]]; then
      echo "FAIL: per-attempt $name has unexpected coverage-trajectory.csv" >&2
      exit 1
    fi
  fi
done

# Final-on-best run produced a trajectory + manifest
test -d "$WORK/out/final"
test -s "$WORK/out/final/manifest.json"
test -s "$WORK/out/final/coverage-trajectory.csv"
[[ "$(head -1 "$WORK/out/final/coverage-trajectory.csv")" == "t_seconds,functions_covered,instantiations_covered,lines_covered,regions_covered,branches_covered,ktests_so_far" ]]

# summary.json points at the final run
[[ "$(jq -r '.final.dir' "$WORK/out/summary.json")" == "final" ]]
[[ "$(jq -r '.final.trajectory' "$WORK/out/summary.json")" == "final/coverage-trajectory.csv" ]]
[[ "$(jq -r '.final.manifest' "$WORK/out/summary.json")" == "final/manifest.json" ]]
[[ "$(jq -r '.final["max-time"]' "$WORK/out/summary.json")" == "10s" ]]

echo "PASS test_eval_symbolon"

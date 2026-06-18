#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EVAL_KLEE="$SCRIPT_DIR/../eval-klee"
# shellcheck source=_klee-shim.sh
. "$SCRIPT_DIR/_klee-shim.sh"
# Inherited by eval-klee → klee child process.
LD_PRELOAD="$(klee_shim_libpthread)"
export LD_PRELOAD

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# 1. Build hello program: instrumented native binary + bitcode
cat >"$WORK/hello.c" <<'EOF'
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
  if (argc < 2) return 1;
  if (strcmp(argv[1], "open") == 0) { printf("sesame\n"); return 0; }
  if (strlen(argv[1]) > 4) { printf("long\n"); return 2; }
  printf("hi %s\n", argv[1]);
  return 0;
}
EOF
clang -O0 -g -fprofile-instr-generate -fcoverage-mapping \
  "$WORK/hello.c" -o "$WORK/hello.cov"
gclang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone \
  "$WORK/hello.c" -o "$WORK/hello.bc"

# 2. Lay out a "baked program" tree the way the image will
PROG="$WORK/program"
mkdir -p "$PROG"
cp "$WORK/hello.bc" "$PROG/program.bc"
cp "$WORK/hello.cov" "$PROG/program.cov"
cat >"$PROG/program.json" <<'EOF'
{
  "name": "hello",
  "version": "0.1.0",
  "with-libcxx": false,
  "sym-env": ["--sym-args", "0", "1", "5"]
}
EOF

# 3. Run eval-klee on the baseline bc
SYMBOLON_PROGRAM_DIR="$PROG" \
  "$EVAL_KLEE" \
  --search dfs \
  --max-time 15s \
  --cov-sample-every 3s \
  --out "$WORK/out"

# 4. Assertions
test -d "$WORK/out/klee-out"
test -s "$WORK/out/coverage/coverage.lcov"
test -s "$WORK/out/coverage/report.json"
test -s "$WORK/out/coverage/summary.txt"
test -d "$WORK/out/coverage/html"
test -s "$WORK/out/manifest.json"

# Manifest sanity
grep -q '"program"' "$WORK/out/manifest.json"
jq -e '.search == "dfs"' "$WORK/out/manifest.json" >/dev/null
grep -q '"input-bc-sha256"' "$WORK/out/manifest.json"
grep -q '"coverage"' "$WORK/out/manifest.json"

# End-to-end: hello has 4 source-line branches under sym-args; DFS for 15s
# should cover at least some of them. This catches silent pipeline regressions
# where files are produced but the coverage merge no-ops.
jq -e '.coverage["lines-covered"] > 0' "$WORK/out/manifest.json" >/dev/null

# Coverage trajectory CSV is produced and consistent with manifest
test -s "$WORK/out/coverage-trajectory.csv"
test -s "$WORK/out/klee-start-epoch.txt"

[[ "$(head -1 "$WORK/out/coverage-trajectory.csv")" == "t_seconds,functions_covered,instantiations_covered,lines_covered,regions_covered,branches_covered,ktests_so_far" ]]

# Final row's lines_covered (col 4) must equal manifest.coverage.lines-covered
manifest_lines=$(jq -r '.coverage["lines-covered"]' "$WORK/out/manifest.json")
csv_final_lines=$(tail -1 "$WORK/out/coverage-trajectory.csv" | cut -d, -f4)
[[ $manifest_lines == "$csv_final_lines" ]]

# Manifest references the trajectory file
[[ "$(jq -r '."coverage-trajectory"' "$WORK/out/manifest.json")" == "coverage-trajectory.csv" ]]

# Manifest exposes covered + total counts for all five metrics. Regions and
# lines must be > 0 for any non-trivial program; functions/instantiations >= 1
# (main itself); branches may be 0 if no conditional source lines were
# exercised, but the field must still be present.
for metric in functions instantiations lines regions branches; do
  jq -e --arg m "$metric" '.coverage[$m + "-covered"] | type == "number"' \
    "$WORK/out/manifest.json" >/dev/null
  jq -e --arg m "$metric" '.coverage[$m + "-total"]   | type == "number"' \
    "$WORK/out/manifest.json" >/dev/null
done
jq -e '.coverage["regions-covered"] > 0' "$WORK/out/manifest.json" >/dev/null
jq -e '.coverage["functions-total"] >= 1' "$WORK/out/manifest.json" >/dev/null

# Final-row covered counts (cols 2..6) must equal manifest's *-covered fields,
# in the same order: functions, instantiations, lines, regions, branches.
csv_final_row="$(tail -1 "$WORK/out/coverage-trajectory.csv")"
i=2
for metric in functions instantiations lines regions branches; do
  manifest_cov=$(jq -r --arg m "$metric" '.coverage[$m + "-covered"]' "$WORK/out/manifest.json")
  csv_cov=$(echo "$csv_final_row" | cut -d, -f$i)
  if [[ $manifest_cov != "$csv_cov" ]]; then
    echo "FAIL: $metric mismatch (manifest=$manifest_cov, csv col $i=$csv_cov)" >&2
    exit 1
  fi
  i=$((i + 1))
done

# --- stop-on-plateau: off-path manifest carries NO stop block (regression) ---
if ! jq -e '.stop == null' "$WORK/out/manifest.json" >/dev/null; then
  echo "FAIL: default (no --stop-on-plateau) manifest unexpectedly has a stop block" >&2
  exit 1
fi

# --- stop-on-plateau: on-path produces an additive stop block ----------------
# hello completes near-instantly, so the conservative watcher reports
# reason=completed; we assert the block's shape, not an early stop.
SYMBOLON_PROGRAM_DIR="$PROG" \
  "$EVAL_KLEE" \
  --search dfs \
  --max-time 30s \
  --cov-sample-every 3s \
  --stop-on-plateau --plateau-window 5s --plateau-poll 1s \
  --out "$WORK/out-sop"

jq -e '.stop.reason          | type == "string"' "$WORK/out-sop/manifest.json" >/dev/null
jq -e '.stop["elapsed-seconds"] | type == "number"' "$WORK/out-sop/manifest.json" >/dev/null
jq -e '.stop.plateau.window == "5s"' "$WORK/out-sop/manifest.json" >/dev/null
jq -e '.stop.plateau["covered-instructions"] | type == "number"' "$WORK/out-sop/manifest.json" >/dev/null
jq -e '.coverage["lines-covered"] > 0' "$WORK/out-sop/manifest.json" >/dev/null
echo "  stop-on-plateau block OK (reason=$(jq -r '.stop.reason' "$WORK/out-sop/manifest.json"))"

echo "PASS test_eval_klee"

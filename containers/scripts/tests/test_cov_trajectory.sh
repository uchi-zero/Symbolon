#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COV_TRAJ="$SCRIPT_DIR/../cov-trajectory"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

EXPECTED_HEADER="t_seconds,functions_covered,instantiations_covered,lines_covered,regions_covered,branches_covered,ktests_so_far"

# Smoke: script is executable and rejects an obviously-invalid invocation
test -x "$COV_TRAJ"

# Missing required args → non-zero exit
set +e
"$COV_TRAJ" 2>/dev/null
rc=$?
set -e
((rc != 0))

# --- Test fixture: build a tiny instrumented binary used by all sub-tests ---
cat >"$WORK/hello.c" <<'EOF'
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
  if (argc < 2) { printf("none\n"); return 1; }
  if (strcmp(argv[1], "a") == 0) { printf("a\n"); return 0; }
  if (strcmp(argv[1], "b") == 0) { printf("b\n"); return 0; }
  printf("other\n");
  return 0;
}
EOF
clang -O0 -g -fprofile-instr-generate -fcoverage-mapping \
  "$WORK/hello.c" -o "$WORK/hello.cov"

# --- Sub-test: empty klee-out → two anchor rows only ---
mkdir -p "$WORK/empty/klee-out" "$WORK/empty/profraw"

"$COV_TRAJ" \
  --klee-out "$WORK/empty/klee-out" \
  --profraw-dir "$WORK/empty/profraw" \
  --binary "$WORK/hello.cov" \
  --max-time-seconds 60 \
  --klee-start-epoch 1700000000 \
  --sample-every 10s \
  --out "$WORK/empty/cov.csv"

# Header + exactly two data rows
[[ "$(head -1 "$WORK/empty/cov.csv")" == "$EXPECTED_HEADER" ]]
[[ "$(wc -l <"$WORK/empty/cov.csv")" == "3" ]]
[[ "$(sed -n 2p "$WORK/empty/cov.csv")" == "0,0,0,0,0,0,0" ]]
[[ "$(sed -n 3p "$WORK/empty/cov.csv")" == "60,0,0,0,0,0,0" ]]

# --- Sub-test: one ktest at t=15s, sampling every 10s, max-time 60s ---
mkdir -p "$WORK/one/klee-out" "$WORK/one/profraw"

# Create a synthetic ktest file with mtime = start + 15s.
START_EPOCH=1700001000
touch "$WORK/one/klee-out/test000001.ktest"
touch -d "@$((START_EPOCH + 15))" "$WORK/one/klee-out/test000001.ktest"

# Generate a matching profraw by running the instrumented binary with
# arg "a" (covers the strcmp(a) branch).
LLVM_PROFILE_FILE="$WORK/one/profraw/test000001.profraw" \
  "$WORK/hello.cov" a >/dev/null

"$COV_TRAJ" \
  --klee-out "$WORK/one/klee-out" \
  --profraw-dir "$WORK/one/profraw" \
  --binary "$WORK/hello.cov" \
  --max-time-seconds 60 \
  --klee-start-epoch "$START_EPOCH" \
  --sample-every 10s \
  --out "$WORK/one/cov.csv"

# Expect: header, anchor (0,0,0,0,0,0,0), event row at t=20 (first tick ≥ 15)
# with all five metrics > 0 and ktests_so_far=1, final anchor at t=60.
[[ "$(head -1 "$WORK/one/cov.csv")" == "$EXPECTED_HEADER" ]]
[[ "$(sed -n 2p "$WORK/one/cov.csv")" == "0,0,0,0,0,0,0" ]]
# Row 3: at the first tick that "sees" the ktest. All metrics should be > 0.
row3="$(sed -n 3p "$WORK/one/cov.csv")"
echo "$row3" | grep -E '^20,[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*,1$' >/dev/null
# Final row sits at t=60 with same metrics as row 3 (no new ktests).
final="$(tail -1 "$WORK/one/cov.csv")"
[[ "$(echo "$final" | cut -d, -f1)" == "60" ]]
[[ "$(echo "$final" | cut -d, -f7)" == "1" ]]
# Metric columns 2..6 must equal row3's
for col in 2 3 4 5 6; do
  [[ "$(echo "$final" | cut -d, -f$col)" == "$(echo "$row3" | cut -d, -f$col)" ]]
done

# --- Sub-test: ktest exists but profraw is missing → arrays end up empty.
#     Regression: must NOT trigger `unbound variable` under set -u.
mkdir -p "$WORK/no-profraw/klee-out" "$WORK/no-profraw/profraw"
START_NP=1700004000
touch "$WORK/no-profraw/klee-out/test000001.ktest"
touch -d "@$((START_NP + 5))" "$WORK/no-profraw/klee-out/test000001.ktest"
# (no profraw written)

"$COV_TRAJ" \
  --klee-out "$WORK/no-profraw/klee-out" \
  --profraw-dir "$WORK/no-profraw/profraw" \
  --binary "$WORK/hello.cov" \
  --max-time-seconds 30 \
  --klee-start-epoch "$START_NP" \
  --sample-every 5s \
  --out "$WORK/no-profraw/cov.csv"

# Anchor-only CSV: header + 0,0,0,0,0,0,0 + 30,0,0,0,0,0,0
[[ "$(wc -l <"$WORK/no-profraw/cov.csv")" == "3" ]]
[[ "$(sed -n 2p "$WORK/no-profraw/cov.csv")" == "0,0,0,0,0,0,0" ]]
[[ "$(sed -n 3p "$WORK/no-profraw/cov.csv")" == "30,0,0,0,0,0,0" ]]

# --- Sub-test: three ktests at t=5,12,40s; ktest #2 covers the same paths as
#     ktest #1 (so it shouldn't add a row), ktest #3 covers a new branch.
mkdir -p "$WORK/multi/klee-out" "$WORK/multi/profraw"
START2=1700002000

touch "$WORK/multi/klee-out/test000001.ktest"
touch "$WORK/multi/klee-out/test000002.ktest"
touch "$WORK/multi/klee-out/test000003.ktest"
touch -d "@$((START2 + 5))" "$WORK/multi/klee-out/test000001.ktest"
touch -d "@$((START2 + 12))" "$WORK/multi/klee-out/test000002.ktest"
touch -d "@$((START2 + 40))" "$WORK/multi/klee-out/test000003.ktest"

LLVM_PROFILE_FILE="$WORK/multi/profraw/test000001.profraw" "$WORK/hello.cov" a >/dev/null
LLVM_PROFILE_FILE="$WORK/multi/profraw/test000002.profraw" "$WORK/hello.cov" a >/dev/null # same as #1
LLVM_PROFILE_FILE="$WORK/multi/profraw/test000003.profraw" "$WORK/hello.cov" b >/dev/null # new branch

"$COV_TRAJ" \
  --klee-out "$WORK/multi/klee-out" \
  --profraw-dir "$WORK/multi/profraw" \
  --binary "$WORK/hello.cov" \
  --max-time-seconds 60 \
  --klee-start-epoch "$START2" \
  --sample-every 10s \
  --out "$WORK/multi/cov.csv"

[[ "$(head -1 "$WORK/multi/cov.csv")" == "$EXPECTED_HEADER" ]]

# At least 4 rows (header + 3 data); at most 5 (an extra t=50 row would be
# unexpected because ktest #3 at t=40 doesn't trigger a tick at t=50).
rows=$(wc -l <"$WORK/multi/cov.csv")
((rows >= 4))
((rows <= 5))

# Anchor (0,0,0,0,0,0,0) is row 2
[[ "$(sed -n 2p "$WORK/multi/cov.csv")" == "0,0,0,0,0,0,0" ]]

# Final row sits at t=60 and has ktests_so_far=3 (last column)
final="$(tail -1 "$WORK/multi/cov.csv")"
[[ "$(echo "$final" | cut -d, -f1)" == "60" ]]
[[ "$(echo "$final" | cut -d, -f7)" == "3" ]]

# Critical dedup assertion: ktest #2 (added at t=20) covers the same path as
# ktest #1, so no metric advances. Row ending in ",2" must not appear.
if grep -qE '^20,.*,2$' "$WORK/multi/cov.csv"; then
  echo "FAIL: dedup t=20 row present (ktest #2 should not have added a row)" >&2
  exit 1
fi

# All five metric columns are monotonic non-decreasing across data rows.
for col in 2 3 4 5 6; do
  prev=0
  while IFS= read -r v; do
    if ((v < prev)); then
      echo "FAIL: metric col $col decreased: $prev -> $v" >&2
      exit 1
    fi
    prev=$v
  done < <(tail -n +2 "$WORK/multi/cov.csv" | cut -d, -f$col)
done

# --- Sub-test: --from-eval-klee-out infers paths from a synthetic eval-klee
#     output dir layout and uses klee-start-epoch.txt as the start time.
mkdir -p "$WORK/from/klee-out" "$WORK/from/profraw"
START3=1700003000

# Required pieces:
#   <dir>/klee-out/             — ktests
#   <dir>/profraw/              — profraws
#   <dir>/manifest.json         — has runtime.max-time
#   <dir>/klee-start-epoch.txt  — Unix seconds
touch "$WORK/from/klee-out/test000001.ktest"
touch -d "@$((START3 + 8))" "$WORK/from/klee-out/test000001.ktest"
LLVM_PROFILE_FILE="$WORK/from/profraw/test000001.profraw" "$WORK/hello.cov" a >/dev/null

cat >"$WORK/from/manifest.json" <<EOF
{
  "runtime": { "max-time": "30s" }
}
EOF
echo "$START3" >"$WORK/from/klee-start-epoch.txt"

"$COV_TRAJ" \
  --from-eval-klee-out "$WORK/from" \
  --binary "$WORK/hello.cov" \
  --sample-every 5s \
  --out "$WORK/from/cov.csv"

# Final row at t=30 (from manifest.runtime.max-time)
[[ "$(tail -1 "$WORK/from/cov.csv" | cut -d, -f1)" == "30" ]]
# At least one event row was emitted (ktest at t=8): seven numeric columns,
# at least one metric > 0.
event_rows=$(tail -n +2 "$WORK/from/cov.csv" |
  grep -cE '^[1-9][0-9]?(,[0-9]+){5},[1-9][0-9]*$')
((event_rows >= 1))

echo "PASS test_cov_trajectory"

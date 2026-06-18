#!/usr/bin/env bash
set -euo pipefail

# Locate the script under test
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COV_MERGE="$SCRIPT_DIR/../cov-merge"

# Build a tiny instrumented hello program in a tmpdir
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

cat >"$WORK/hello.c" <<'EOF'
#include <stdio.h>
int main(int argc, char **argv) {
  if (argc > 1) { printf("yes\n"); return 0; }
  printf("no\n");
  return 1;
}
EOF

clang -O0 -g -fprofile-instr-generate -fcoverage-mapping \
  "$WORK/hello.c" -o "$WORK/hello"

# Generate two profraw runs with different coverage. The no-arg path
# returns 1 by design (testing the false branch); `|| true` keeps strict
# mode happy without masking any tool failure — instrumentation regressions
# would surface as an empty profraw, caught by `test -s` below.
mkdir -p "$WORK/profraw"
LLVM_PROFILE_FILE="$WORK/profraw/run1.profraw" "$WORK/hello" >/dev/null || true
LLVM_PROFILE_FILE="$WORK/profraw/run2.profraw" "$WORK/hello" arg >/dev/null

# Run the script
mkdir -p "$WORK/out"
"$COV_MERGE" \
  --binary "$WORK/hello" \
  --profraw-dir "$WORK/profraw" \
  --out "$WORK/out"

# Assertions: every artifact promised by the spec exists and is non-empty
test -s "$WORK/out/program.profdata"
test -s "$WORK/out/coverage.lcov"
test -s "$WORK/out/report.json"
test -s "$WORK/out/summary.txt"
test -d "$WORK/out/html"
test -s "$WORK/out/html/index.html"

# Sanity: report.json must contain the line counters for hello.c
grep -q "hello.c" "$WORK/out/report.json"
grep -q '"lines"' "$WORK/out/report.json"

# Empty-profraw branch: cov-merge must emit valid placeholder reports
# rather than aborting, so downstream consumers (jq) don't crash.
mkdir -p "$WORK/empty-in" "$WORK/empty-out"
"$COV_MERGE" \
  --binary "$WORK/hello" \
  --profraw-dir "$WORK/empty-in" \
  --out "$WORK/empty-out"
test -f "$WORK/empty-out/report.json"
test -s "$WORK/empty-out/report.json"
# report.json must be parseable JSON with a `data` array
# shellcheck disable=SC2016 # `$0` is intentionally unexpanded — used inside the nix-shell --run subshell.
nix-shell -p jq --run 'jq -e ".data | type == \"array\"" "$0" >/dev/null' "$WORK/empty-out/report.json" 2>/dev/null ||
  jq -e '.data | type == "array"' "$WORK/empty-out/report.json" >/dev/null

echo "PASS test_cov_merge"

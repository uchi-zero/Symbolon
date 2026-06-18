#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPLAY_ALL="$SCRIPT_DIR/../klee-replay-all"
# shellcheck source=_klee-shim.sh
. "$SCRIPT_DIR/_klee-shim.sh"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# Build an instrumented hello program
cat >"$WORK/hello.c" <<'EOF'
#include <stdio.h>
int main(int argc, char **argv) {
  if (argc > 1 && argv[1][0] == 'y') return 0;
  return 1;
}
EOF
clang -O0 -g -fprofile-instr-generate -fcoverage-mapping \
  "$WORK/hello.c" -o "$WORK/hello"

# Build a bitcode for KLEE so we get real ktests
gclang -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone "$WORK/hello.c" -o "$WORK/hello.bc"

# Run KLEE briefly to produce a klee-out dir. klee_shim() handles the
# glibc 2.40/2.42 LD_PRELOAD workaround documented in _klee-shim.sh.
mkdir -p "$WORK/klee-out"
klee_shim --output-dir="$WORK/klee-out/run" --posix-runtime --libc=uclibc \
  --max-time=10s "$WORK/hello.bc" --sym-args 0 1 1 || true
ls "$WORK/klee-out/run"/*.ktest 1>/dev/null

mkdir -p "$WORK/profraw"
"$REPLAY_ALL" \
  --binary "$WORK/hello" \
  --klee-out "$WORK/klee-out/run" \
  --profraw-dir "$WORK/profraw" \
  --replays-jsonl "$WORK/replays.jsonl" \
  --timeout 5s

# At least one profraw produced (== at least one ktest replayed)
shopt -s nullglob
prs=("$WORK/profraw"/*.profraw)
((${#prs[@]} > 0))

# replays.jsonl has one line per ktest
test -s "$WORK/replays.jsonl"
lines=$(wc -l <"$WORK/replays.jsonl")
ktests=$(find "$WORK/klee-out/run" -maxdepth 1 -name '*.ktest' | wc -l)
[[ $lines == "$ktests" ]]

# Each line is JSON with required fields
head -1 "$WORK/replays.jsonl" | grep -q '"ktest"'
head -1 "$WORK/replays.jsonl" | grep -q '"exit"'
head -1 "$WORK/replays.jsonl" | grep -q '"profraw"'

echo "PASS test_klee_replay_all"

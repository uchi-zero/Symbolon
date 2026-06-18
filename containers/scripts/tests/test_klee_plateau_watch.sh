#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WATCH="$SCRIPT_DIR/../klee-plateau-watch"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# Build a fake klee-out: N empty .ktest files + a minimal run.stats SQLite
# carrying a single CoveredInstructions value. The watcher reads ktest COUNT
# (idle = count not increasing) and the latest CoveredInstructions row.
mk_klee_out() { # $1=dir  $2=num_ktests  $3=covered_instructions
  local d="$1" n="$2" ci="$3" i
  mkdir -p "$d"
  for ((i = 0; i < n; i++)); do
    : > "$d/test$(printf '%06d' "$i").ktest"
  done
  sqlite3 "$d/run.stats" \
    "CREATE TABLE stats(CoveredInstructions INTEGER); INSERT INTO stats VALUES($ci);"
}

set_ci() { # $1=dir  $2=value
  sqlite3 "$1/run.stats" "UPDATE stats SET CoveredInstructions=$2;" 2>/dev/null || true
}

# --- Case 1: plateau (no new ktest, CoveredInstructions flat) ---------------
(
  d="$WORK/c1"; mk_klee_out "$d" 2 100
  sleep 600 & dummy=$!
  trap 'kill "$dummy" 2>/dev/null || true' EXIT
  rf="$WORK/c1.reason"
  "$WATCH" --klee-out "$d" --pid "$dummy" \
    --window 2s --poll 1s --ceiling 0 --grace 2s --reason-file "$rf"
  if [[ "$(cat "$rf")" != "plateau" ]]; then echo "c1 FAIL: reason=$(cat "$rf")" >&2; exit 1; fi
  if kill -0 "$dummy" 2>/dev/null; then echo "c1 FAIL: worker still alive" >&2; exit 1; fi
  echo "  c1 plateau OK"
)

# --- Case 2: keep-going guard (CoveredInstructions still climbing) ----------
# ktests idle, but CI rises every second for ~6s. The guard must prevent a
# plateau stop during the climb; the watcher must NOT have stopped before ~6s.
(
  d="$WORK/c2"; mk_klee_out "$d" 2 100
  sleep 600 & dummy=$!
  trap 'kill "$dummy" 2>/dev/null || true' EXIT
  ( for i in $(seq 1 6); do set_ci "$d" $((100 + i)); sleep 1; done ) & updater=$!
  rf="$WORK/c2.reason"
  t0=$(date +%s)
  "$WATCH" --klee-out "$d" --pid "$dummy" \
    --window 2s --poll 1s --ceiling 30 --grace 2s --reason-file "$rf"
  t1=$(date +%s); elapsed=$((t1 - t0))
  kill "$updater" 2>/dev/null || true
  if [[ "$(cat "$rf")" != "plateau" ]]; then echo "c2 FAIL: reason=$(cat "$rf")" >&2; exit 1; fi
  if (( elapsed < 6 )); then echo "c2 FAIL: stopped too early ($elapsed s < 6)" >&2; exit 1; fi
  echo "  c2 keep-going guard OK ($elapsed s)"
)

# --- Case 3: worker exits on its own (KLEE finished before plateau) ----------
(
  d="$WORK/c3"; mk_klee_out "$d" 1 50
  sleep 1 & dummy=$!
  trap 'kill "$dummy" 2>/dev/null || true' EXIT
  rf="$WORK/c3.reason"
  "$WATCH" --klee-out "$d" --pid "$dummy" \
    --window 30s --poll 1s --ceiling 0 --grace 2s --reason-file "$rf"
  if [[ "$(cat "$rf")" != "exited" ]]; then echo "c3 FAIL: reason=$(cat "$rf")" >&2; exit 1; fi
  echo "  c3 exited OK"
)

# --- Case 4: ceiling stop fires before plateau ------------------------------
(
  d="$WORK/c4"; mk_klee_out "$d" 2 100
  sleep 600 & dummy=$!
  trap 'kill "$dummy" 2>/dev/null || true' EXIT
  rf="$WORK/c4.reason"
  "$WATCH" --klee-out "$d" --pid "$dummy" \
    --window 60s --poll 1s --ceiling 3 --grace 2s --reason-file "$rf"
  if [[ "$(cat "$rf")" != "ceiling" ]]; then echo "c4 FAIL: reason=$(cat "$rf")" >&2; exit 1; fi
  if kill -0 "$dummy" 2>/dev/null; then echo "c4 FAIL: worker still alive" >&2; exit 1; fi
  echo "  c4 ceiling OK"
)

# --- Case 5: the watcher is packaged into the image -------------------------
# eval-klee launches it via "$(dirname "$0")/klee-plateau-watch", so it must be
# installed next to eval-klee. The behavioural tests run from the source tree
# (where the sibling always exists), so guard the install list explicitly.
INSTALL_NIX="$SCRIPT_DIR/../../lib/shared-layers.nix"
if ! grep -q 'klee-plateau-watch' "$INSTALL_NIX"; then
  echo "c5 FAIL: klee-plateau-watch not installed in $INSTALL_NIX" >&2
  exit 1
fi
echo "  c5 packaged OK"

echo "PASS test_klee_plateau_watch"

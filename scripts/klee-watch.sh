#!/usr/bin/env bash
# klee-watch.sh — run eval-klee (or eval-transformations) in the background and
# SIGTERM it the moment the run reaches a clear verdict, per TRANSFORMATION.md §0.
#
# Verdicts written to <out>/watch-verdict.txt:
#   saturated  — CoveredInstructions flat for FLAT_WINDOW and heap well below cap
#   starved    — CoveredInstructions flat but heap pinned near --max-memory cap
#   (a run that hits --max-time without either is "incomplete")
#
# Usage:
#   klee-watch.sh <cap_mb> <out_dir> -- <eval-cmd...>
# Example:
#   klee-watch.sh 32000 /tmp/run -- eval-klee --search nurs:covnew \
#       --max-time=4h --max-memory=32000 --cov-sample-every=5m --out /tmp/run
set -uo pipefail

cap_mb="$1"
out="$2"
shift 2
[[ $1 == "--" ]] && shift

POLL=30                      # seconds between samples
MIN_WALL=300                 # 5 min warmup
FLAT_WINDOW=900              # 15 min of unchanged CoveredInstructions = plateau (wide; nurs can burst)
STARVE_MB=$((cap_mb - 1500)) # heap within 1.5 GB of cap = pressure
STARVE_HOLD=300              # heap must stay high this long while flat to call it starved

mkdir -p "$out"
STATS="$out/klee-out/run.stats"
rm -f "$out/watch-verdict.txt"

"$@" &
EVAL_PID=$!
echo "watcher: launched eval pid=$EVAL_PID cap=${cap_mb}MB out=$out" >&2

# Graceful halt: SIGINT the klee executor(s) like Ctrl-C. KLEE stops forking,
# flushes covering ktests, and returns — eval-klee then runs its
# replay/cov-merge/cov-trajectory/manifest pipeline naturally. We do NOT kill
# eval-klee itself (that would orphan klee and skip the pipeline).
halt_klee() {
  echo "$1" >"$out/watch-verdict.txt"
  echo "watcher: $1 — SIGINT klee, letting eval-klee finalize" >&2
  pkill -INT -x klee 2>/dev/null || true
}

last_change_s=0
prev_covered=-1
high_heap_since=-1
halted=0

while kill -0 "$EVAL_PID" 2>/dev/null; do
  sleep "$POLL"
  ((halted)) && continue # klee already signalled; just wait for pipeline
  [[ -s $STATS ]] || continue

  # run.stats stores WallTime/MallocUsage as REAL; compute integers in SQL
  # (CAST to INTEGER) so bash arithmetic never sees a decimal point.
  row=$(sqlite3 -separator ' ' "$STATS" \
    "SELECT CAST(WallTime/1000000 AS INTEGER), CoveredInstructions,
            CAST(MallocUsage/1048576 AS INTEGER), InhibitedForks, NumStates
     FROM stats ORDER BY WallTime DESC LIMIT 1;" 2>/dev/null) || continue
  [[ -n $row ]] || continue
  read -r wall_s covered heap_mb inhibited nstates <<<"$row"
  [[ $covered =~ ^[0-9]+$ && $wall_s =~ ^[0-9]+$ && $heap_mb =~ ^[0-9]+$ ]] || continue

  if ((covered != prev_covered)); then
    last_change_s=$wall_s
    prev_covered=$covered
  fi
  flat_for=$((wall_s - last_change_s))

  # track how long heap has been high
  if ((heap_mb > STARVE_MB)); then
    ((high_heap_since < 0)) && high_heap_since=$wall_s
  else
    high_heap_since=-1
  fi

  printf 'watcher: t=%ds covered=%d heap=%dMB states=%d inhibited=%d flat=%ds\n' \
    "$wall_s" "$covered" "$heap_mb" "$nstates" "$inhibited" "$flat_for" >&2

  ((wall_s < MIN_WALL)) && continue

  # Starvation: flat AND heap pinned near cap for a sustained window.
  if ((flat_for >= STARVE_HOLD)) && ((high_heap_since >= 0)) &&
    ((wall_s - high_heap_since >= STARVE_HOLD)); then
    halt_klee starved
    halted=1
    continue
  fi

  # Saturation: flat for FLAT_WINDOW and heap comfortably below cap.
  if ((flat_for >= FLAT_WINDOW)) && ((heap_mb <= STARVE_MB)); then
    halt_klee saturated
    halted=1
    continue
  fi
done

wait "$EVAL_PID"
ev=$?
[[ -f "$out/watch-verdict.txt" ]] || echo "incomplete-or-finished" >"$out/watch-verdict.txt"
echo "watcher: eval exited ($ev); verdict=$(cat "$out/watch-verdict.txt")" >&2

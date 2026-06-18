#!/usr/bin/env bash
# sat-watch.sh — run an eval-klee / eval-transformations command under the
# live-saturation watcher from TRANSFORMATION.md §0. SIGTERMs the eval as soon
# as CoveredInstructions in run.stats has been flat for FLAT_WINDOW and the run
# is NOT starved (heap below cap, forks not being inhibited). SIGTERM lets the
# watchdog-wrapped KLEE finalise ktests and run the replay/cov-merge pipeline.
#
# Usage:
#   sat-watch.sh <out-dir> -- <eval command...>
# Example:
#   sat-watch.sh /tmp/run-readelf-baseline -- \
#     env SYMBOLON_PROGRAM_DIR=$PROG containers/scripts/eval-klee \
#       --search nurs:covnew --max-time=4h --max-memory=8000 \
#       --cov-sample-every=5m --out /tmp/run-readelf-baseline
#
# Env knobs (override before calling):
#   POLL=30         seconds between samples
#   MIN_WALL=300    warmup before saturation can be declared (s)
#   FLAT_WINDOW=600 unchanged CoveredInstructions => saturated (s)
#   HEAP_CAP_MB=7000   heap above this => treat as starved, don't stop
#   INHIBIT_CAP=1000   inhibited forks above this => starved, don't stop
set -uo pipefail

out="$1"
shift
[[ $1 == "--" ]] && shift

POLL="${POLL:-30}"
MIN_WALL="${MIN_WALL:-300}"
FLAT_WINDOW="${FLAT_WINDOW:-600}"
HEAP_CAP_MB="${HEAP_CAP_MB:-7000}"
INHIBIT_CAP="${INHIBIT_CAP:-1000}"

mkdir -p "$out"
"$@" &
EVAL_PID=$!
STATS="$out/klee-out/run.stats"

# Find the KLEE *worker* process (the grandchild under --watchdog). KLEE's own
# watchdog halts the worker via SIGTERM, so signalling the worker triggers the
# identical clean-halt path: pending ktests are finalised, the watchdog sees a
# clean exit, and eval-klee's foreground `klee` returns and proceeds to the
# replay/cov-merge pipeline. Signalling eval-klee (bash) instead would kill the
# script before that pipeline runs.
klee_worker() {
  ps -eo pid,ppid,comm | awk '$3=="klee"{p[$1]=$2} END{for(pid in p){pp=p[pid]; if(pp in p) print pid}}'
}

# Timing is anchored to REAL wall-clock (date +%s), not run.stats' WallTime
# column: on the section-table stall KLEE stops flushing run.stats entirely,
# so a stats-derived clock freezes and the flat-window never elapses. Real
# time keeps ticking, so a stalled run is correctly seen as "covered flat for
# FLAT_WINDOW" and stopped. CoveredInstructions/heap/inhibited are still read
# from stats (best-effort; a frozen value just reads as flat).
klee_start=0
last_change_wall=0
prev_covered=-1
verdict="unknown"

stop_worker() {
  local worker
  worker="$(klee_worker)"
  echo "watcher: stopping klee worker(s) [$worker]"
  for w in $worker; do kill -TERM "$w" 2>/dev/null; done
  # A worker wedged inside one long solver/memory op won't see SIGTERM until
  # the op returns; SIGKILL after a grace period so eval-klee can still replay
  # the ktests already written for completed (new-coverage) states.
  for _ in $(seq 1 12); do
    sleep 5
    klee_worker | grep -q . || return 0
  done
  worker="$(klee_worker)"
  echo "watcher: worker still wedged — SIGKILL [$worker]"
  for w in $worker; do kill -KILL "$w" 2>/dev/null; done
}

while kill -0 "$EVAL_PID" 2>/dev/null; do
  sleep "$POLL"
  [[ -s $STATS ]] || continue
  now=$(date +%s)
  ((klee_start == 0)) && {
    klee_start=$now
    last_change_wall=$now
  }

  read -r covered heap_mb inhibited < <(sqlite3 -separator ' ' "$STATS" \
    "SELECT CAST(CoveredInstructions AS INTEGER),
            CAST(MallocUsage/1048576 AS INTEGER),
            CAST(InhibitedForks AS INTEGER)
     FROM stats ORDER BY WallTime DESC LIMIT 1;" 2>/dev/null) || continue
  [[ -n ${covered:-} ]] || continue

  if [[ $covered != "$prev_covered" ]]; then
    last_change_wall=$now
    prev_covered=$covered
  fi
  wall_s=$((now - klee_start))
  flat_for=$((now - last_change_wall))

  printf 'watcher: wall=%ds covered=%s heap=%sMB inhibited=%s flat_for=%ds\n' \
    "$wall_s" "$covered" "$heap_mb" "$inhibited" "$flat_for"

  ((wall_s < MIN_WALL)) && continue
  ((flat_for < FLAT_WINDOW)) && continue

  if ((heap_mb > HEAP_CAP_MB)) || ((inhibited > INHIBIT_CAP)); then
    echo "watcher: flat but STARVED (heap=${heap_mb}MB inhibited=${inhibited}) — letting it run out"
    verdict="starved"
    continue
  fi

  echo "watcher: SATURATED (covered flat ${flat_for}s, wall=${wall_s}s)"
  verdict="saturated"
  stop_worker
  break
done

wait "$EVAL_PID"
eval_rc=$?
echo "watcher: eval exited rc=$eval_rc verdict=$verdict"
exit "$eval_rc"

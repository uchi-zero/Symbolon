#!/usr/bin/env bash
# shellcheck disable=SC2154  # argc_* vars are assigned via `eval "$(argc ...)"`

# run-klee-searchers — drive a list of KLEE searchers against a baked
# klee-eval-<program>:latest image, running N podman containers in parallel.
# Restartable: skips any searcher whose manifest.json already exists. Each
# container's combined stdout/stderr is tee'd to <out-root>/<searcher>.driver.log.
#
# Recommended invocation (survives ssh disconnects):
#   tmux new -d -s sweep "scripts/run-klee-searchers --program bash"
#
# Live progress for one searcher:
#   tail -f ~/bash-16-searchers/dfs.driver.log

# @option --program!              Program name (e.g. bash, curl, sed). Drives image and out-root defaults.
# @option --image                 Image to run (default: klee-eval-<program>:latest).
# @option --out-root              Output root dir (default: $HOME/<program>-16-searchers).
# @option --max-time=12h          KLEE wall-clock budget per searcher (Ns/Nm/Nh).
# @option --max-memory=32768      KLEE --max-memory (MB; soft cap on KLEE's state heap).
# @option --cov-sample-every=30m  Coverage trajectory sample interval. 0 disables.
# @option --podman-memory=45g     Per-container memory cap (cgroup hard limit).
# @option --parallel=4            Containers in flight at once.
# @option --searcher*             Searcher to run (repeatable). Defaults to all 16 KLEE searchers.

set -euo pipefail
eval "$(argc --argc-eval "$0" "$@")"

program="$argc_program"
image="${argc_image:-klee-eval-${program}:latest}"
out_root="${argc_out_root:-$HOME/${program}-16-searchers}"
max_time="$argc_max_time"
max_memory="$argc_max_memory"
cov_sample_every="$argc_cov_sample_every"
podman_memory="$argc_podman_memory"
parallel="$argc_parallel"

DEFAULT_SEARCHERS=(
  dfs
  bfs
  random-state
  random-path
  nurs:covnew
  nurs:md2u
  nurs:depth
  nurs:rp
  nurs:icnt
  nurs:cpicnt
  nurs:qc
  sgs
  cgs
  cbc
  empc
  learch
)

searchers=("${argc_searcher[@]+"${argc_searcher[@]}"}")
if ((${#searchers[@]} == 0)); then
  searchers=("${DEFAULT_SEARCHERS[@]}")
fi

# ---- Preflight ----------------------------------------------------------
command -v podman >/dev/null || {
  echo "run-klee-searchers: podman not on PATH" >&2
  exit 2
}
command -v jq >/dev/null || {
  echo "run-klee-searchers: jq not on PATH" >&2
  exit 2
}
podman image exists "$image" || {
  cat >&2 <<EOF
run-klee-searchers: image $image not in containers-storage. Build + load with:
  nix build .#klee-eval-${program}.copyTo
  ./result/bin/copy-to containers-storage:${image}
EOF
  exit 2
}

mkdir -p "$out_root"

# ---- Per-searcher runner ------------------------------------------------
# Always returns 0 so a single searcher's failure doesn't abort the batch
# under xargs -P. Failures are visible in the final summary (no manifest).
run_one() {
  local s="$1"
  local safe="${s//:/_}" # nurs:covnew → nurs_covnew (filesystem-safe)
  local out="$out_root/$safe"
  local log="$out_root/$safe.driver.log"

  mkdir -p "$out"

  if [[ -s "$out/manifest.json" ]]; then
    printf '[%s] skip (manifest.json exists)\n' "$s" | tee -a "$log"
    return 0
  fi

  {
    printf '[%s] start %s\n' "$s" "$(date -Iseconds)"
    printf '[%s] image=%s podman-mem=%s klee-max=%sMB max-time=%s sample=%s\n' \
      "$s" "$image" "$podman_memory" "$max_memory" "$max_time" "$cov_sample_every"
  } | tee -a "$log"

  set +e
  podman run --rm \
    --memory="$podman_memory" \
    --memory-swap="$podman_memory" \
    --name "klee-sweep-${program}-${safe}-$$" \
    -v "$out":/out:Z \
    "$image" \
    --search "$s" \
    --max-time "$max_time" \
    --max-memory "$max_memory" \
    --cov-sample-every "$cov_sample_every" \
    --out /out \
    >>"$log" 2>&1
  local rc=$?
  set -e

  printf '[%s] done %s exit=%d\n' "$s" "$(date -Iseconds)" "$rc" | tee -a "$log"
  return 0
}
export -f run_one
export out_root image max_time max_memory cov_sample_every podman_memory program

# ---- Launch -------------------------------------------------------------
printf '%s\n' "${searchers[@]}" |
  xargs -P "$parallel" -I {} bash -c 'run_one "$@"' _ {}

# ---- Summary ------------------------------------------------------------
echo
echo "==== Summary (${out_root}) ===="
summary_fmt='%-15s %10s %10s %10s %10s %10s\n'
# shellcheck disable=SC2059
printf "$summary_fmt" searcher functions instantiations lines regions branches
for s in "${searchers[@]}"; do
  safe="${s//:/_}"
  m="$out_root/$safe/manifest.json"
  if [[ -s $m ]]; then
    read -r f i l r b < <(jq -r '
      .coverage as $c |
      "\($c["functions-covered"]) \($c["instantiations-covered"]) \($c["lines-covered"]) \($c["regions-covered"]) \($c["branches-covered"])"
    ' "$m")
    # shellcheck disable=SC2059
    printf "$summary_fmt" "$s" "$f" "$i" "$l" "$r" "$b"
  else
    printf '%-15s %10s\n' "$s" "MISSING"
  fi
done

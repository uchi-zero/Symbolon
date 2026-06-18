#!/usr/bin/env bash
set -euo pipefail

cd "$(git rev-parse --show-toplevel)"

# We require podman; docker fallback removed (host has only podman).
command -v podman >/dev/null ||
  {
    echo "podman not on PATH" >&2
    exit 2
  }
command -v nix >/dev/null ||
  {
    echo "nix not on PATH" >&2
    exit 2
  }

# Build the image's copyTo helper and load it into podman's containers-storage.
# nix2container exposes a .copyTo output that is a directory with bin/copy-to.
# Building only that output places it at ./result; the script embeds a reference
# to the correct image JSON, so no separate build of the default output is needed.
nix build .#klee-eval-hello.copyTo
./result/bin/copy-to containers-storage:klee-eval-hello:latest

# ---- 1. Baseline run ----
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
mkdir -p "$WORK/out"

podman run --rm --network=none \
  -v "$WORK/out":/out:Z \
  klee-eval-hello:latest \
  --search dfs --max-time 30s --out /out

test -s "$WORK/out/manifest.json"
test -s "$WORK/out/coverage/report.json"
covered=$(jq -r '.coverage["lines-covered"]' "$WORK/out/manifest.json")
[[ $covered -gt 0 ]] ||
  {
    echo "expected >0 lines covered, got $covered" >&2
    exit 1
  }

echo "baseline OK: $covered lines covered"

# ---- 2. Symbolon run with stub agent ----
AGENT_DIR="$WORK/agent"
mkdir -p "$AGENT_DIR"
cp containers/scripts/tests/fixtures/stub-agent/main.sh "$AGENT_DIR/main.sh"
chmod +x "$AGENT_DIR/main.sh"

mkdir -p "$WORK/out2"
podman run --rm \
  -v "$AGENT_DIR":/agent:ro,Z \
  -v "$WORK/out2":/out:Z \
  --entrypoint /bin/eval-symbolon \
  klee-eval-hello:latest \
  --time-budget 90s --max-attempts 2 \
  --search dfs --max-time-per-attempt 15s \
  --agent-cmd "/agent/main.sh" --out /out

test -s "$WORK/out2/summary.json"
test -d "$WORK/out2/attempts/000-baseline"

# Symbolon mode mounted /opt/program/src as the baked (empty placeholder)
# source dir. The stub agent will copy 0 bytes; the agent's behavior is
# tested separately. Just verify the harness ran the loop end-to-end.
mapfile -t attempts < <(find "$WORK/out2/attempts" -mindepth 1 -maxdepth 1 -type d -name '[0-9]*' | sort)
((${#attempts[@]} >= 1)) ||
  {
    echo "expected at least 1 attempt dir, got ${#attempts[@]}" >&2
    exit 1
  }

echo "symbolon OK: ${#attempts[@]} attempt dir(s)"
echo "PASS e2e_hello"

#!/usr/bin/env bash
# Stub agent: copies SYMBOLON_PROGRAM_SRC verbatim to SYMBOLON_WORK_DIR/src
# and writes a trivial rationale. Used in tests as a deterministic substitute
# for the real Claude Agent SDK agent.
set -euo pipefail
mkdir -p "$SYMBOLON_WORK_DIR/src"
cp -r "$SYMBOLON_PROGRAM_SRC"/. "$SYMBOLON_WORK_DIR/src"/
cat >"$SYMBOLON_WORK_DIR/agent-rationale.md" <<EOF
Stub agent: returned source unchanged.
attempts dir: $SYMBOLON_ATTEMPTS_DIR
EOF

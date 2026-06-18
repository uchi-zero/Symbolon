#!/usr/bin/env bash
# Regenerate the fork-finder fixture goldens from their inputs.
# Requires that pass-fixture-runner and fork-finder have been built first
# (cmake --build pass/build --target fork-finder pass-fixture-runner).
set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
build_dir=${BUILD_DIR:-$script_dir/../../build}

runner=$build_dir/common/pass-fixture-runner
binary=$build_dir/fork-finder/fork-finder

if [[ ! -x $runner ]]; then
  echo "error: $runner not found or not executable; build it first" >&2
  exit 1
fi
if [[ ! -x $binary ]]; then
  echo "error: $binary not found or not executable; build it first" >&2
  exit 1
fi

exec "$runner" \
  --tool=fork-finder \
  --binary="$binary" \
  --inputs="$script_dir/inputs" \
  --golden="$script_dir/golden" \
  --update-golden \
  "$@"

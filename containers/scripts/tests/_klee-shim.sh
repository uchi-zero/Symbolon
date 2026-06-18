# shellcheck shell=bash
# Shared helper for tests that invoke `klee` directly (not `klee-replay`).
# Source this from a test script with `. "$SCRIPT_DIR/_klee-shim.sh"`.
#
# Background: in the parent flake's dev shell, `klee` fails at runtime with
#   GLIBC_ABI_DT_X86_64_PLT not found (required by .../glibc-2.42/libpthread.so.0)
# because gperftools (a klee dep) pulls in glibc-2.42's libpthread while klee
# itself is linked against glibc-2.40's libc.so.6. The mismatch is harmless
# inside the OCI image we'll build in later tasks (closure is consistent), but
# breaks `klee` invocation in `nix develop`.
#
# Workaround: preload glibc-2.40's libpthread.so.0, derived dynamically from
# `klee-replay`'s linker resolution (which resolves to the correct glibc 2.40).
# Avoids hardcoding a /nix/store path that would rot on flake updates.

klee_shim_libpthread() {
  local libc_path
  libc_path="$(ldd "$(command -v klee-replay)" 2>/dev/null |
    awk '/libc\.so\.6/ {print $3; exit}')"
  if [[ -z $libc_path || ! -f $libc_path ]]; then
    echo "klee-shim: could not resolve klee-replay's libc.so.6 via ldd" >&2
    return 1
  fi
  echo "$(dirname "$libc_path")/libpthread.so.0"
}

klee_shim() {
  local libpthread
  libpthread="$(klee_shim_libpthread)"
  LD_PRELOAD="$libpthread" klee "$@"
}

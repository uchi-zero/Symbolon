{pkgs, ...}: {
  name = "hello";
  version = "0.1.0";
  notes = "";

  src = ./source;

  # Plain Makefile, no configure step. The phase honors $CC/$CFLAGS exported
  # by program-builder.
  build = {
    artifact = "hello";
    phase = "make -j$NIX_BUILD_CORES hello";
  };

  # Always built with --posix-runtime --libc=uclibc; the only program-level
  # toggle is whether to also pass --libcxx (for C++ programs).
  withLibcxx = false;

  # Symbolic-environment args appended after the bitcode in the KLEE
  # invocation: e.g. ["--sym-args" "0" "1" "5"], ["--sym-stdin" "8"], etc.
  # These define what KLEE makes symbolic for this program.
  symEnv = ["--sym-args" "0" "1" "5"];
}

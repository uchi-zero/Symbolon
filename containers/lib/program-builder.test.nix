{
  pkgs,
  programBuilder,
  helloHarness,
  progDir,
}: let
  built = programBuilder {inherit progDir;} helloHarness;
in
  pkgs.runCommand "program-builder-test-hello" {
    nativeBuildInputs = [pkgs.llvmPackages_klee.llvm pkgs.file];
  } ''
    set -euo pipefail

    # Verify the bitcode artifact exists and is non-trivial
    test -s ${built.baselineBc}/program.bc
    file ${built.baselineBc}/program.bc | grep -q "LLVM IR bitcode"

    # Verify the instrumented binary exists, runs, and has coverage mapping
    test -x ${built.instrumentedBin}/program.cov
    LLVM_PROFILE_FILE=$TMPDIR/test.profraw \
      ${built.instrumentedBin}/program.cov open
    test -s $TMPDIR/test.profraw

    # Verify the program profile JSON is valid and contains expected keys
    test -s ${built.programJson}
    grep -q '"with-libcxx"' ${built.programJson}
    grep -q '"sym-env"' ${built.programJson}

    # patchedSrcDrv contains the patched source tree
    test -d ${built.patchedSrcDrv}
    test -s ${built.patchedSrcDrv}/hello.c
    test -s ${built.patchedSrcDrv}/Makefile

    touch $out
  ''

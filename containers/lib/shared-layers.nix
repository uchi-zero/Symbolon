{
  pkgs,
  kleePkgs,
}: let
  llvmPkgs = pkgs.llvmPackages_klee;
in {
  # Three shared layers, ordered by update cadence (least → most volatile).
  #
  #   base    — runtime tools + LLVM 16 + gllvm + uv. Updates only on a
  #             nixpkgs revision bump (rare in this project).
  #   klee    — klee + klee-libcxx + klee-wrappers from the project's own
  #             KLEE fork. Updates whenever feat/symbolon advances.
  #   scripts — the bash entrypoints in this repo. Updates when this
  #             repo's scripts change.
  #
  # Use kleePkgs (klee flake's own packages output) rather than pkgs.klee from
  # the overlay. The overlay-provided klee can pick up a minisat built against
  # a newer glibc than the klee binary itself, causing a GLIBC_ABI_DT_X86_64_PLT
  # version conflict at runtime.
  base = pkgs.buildEnv {
    name = "klee-eval-base";
    paths = with pkgs; [
      argc
      bashInteractive
      coreutils
      gnused
      gnugrep
      gawk
      findutils
      gnumake
      diffutils
      file
      glibc.bin
      patch
      jq
      cacert
      llvmPkgs.clang
      llvmPkgs.lld
      llvmPkgs.llvm
      gllvm
      uv
    ];
  };

  klee = pkgs.buildEnv {
    name = "klee-eval-klee";
    paths = [
      kleePkgs.klee
      kleePkgs.klee-libcxx
      kleePkgs.klee-wrappers
    ];
  };

  scripts = pkgs.stdenv.mkDerivation {
    name = "klee-eval-scripts";
    src = ../scripts;
    dontConfigure = true;
    dontBuild = true;
    installPhase = ''
      mkdir -p $out/bin
      install -m 0755 eval-klee             $out/bin/
      install -m 0755 eval-symbolon         $out/bin/
      install -m 0755 klee-replay-all       $out/bin/
      install -m 0755 klee-plateau-watch    $out/bin/
      install -m 0755 cov-merge             $out/bin/
      install -m 0755 cov-trajectory        $out/bin/
    '';
  };
}

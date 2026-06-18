{
  pkgs,
  n2c,
  kleePkgs,
}: let
  inherit (pkgs) lib;

  programBuilder = import ./lib/program-builder.nix {inherit pkgs kleePkgs;};
  sharedLayers = import ./lib/shared-layers.nix {inherit pkgs kleePkgs;};
  buildImage = import ./lib/eval-image.nix {inherit pkgs n2c sharedLayers;};

  # Auto-discover every subdirectory of ./programs as a harness. Each harness
  # is a function `{pkgs, ...}: <attrset>`; it pulls whatever it needs from
  # `pkgs` itself, so adding a program means only dropping a new directory
  # under ./programs — no edits here.
  programNames =
    lib.attrNames
    (lib.filterAttrs (_: type: type == "directory") (builtins.readDir ./programs));

  # Each program yields:
  #   klee-eval-<name>        — baseline program.bc eval image
  #   klee-eval-<name>-asan   — standalone ASan binary
  #   klee-eval-<name>-ubsan  — standalone UBSan binary
  buildProgram = name: let
    progDir = ./programs + "/${name}";
    harness = import progDir {inherit pkgs;};
    baselineBuilt = programBuilder {inherit progDir;} harness;
  in [
    {
      name = "klee-eval-${name}";
      value = buildImage baselineBuilt;
    }
    # Standalone sanitizer binaries (baseline = original program), for host-side
    # ktest replay without building a full eval image.
    {
      name = "klee-eval-${name}-asan";
      value = baselineBuilt.asanBin;
    }
    {
      name = "klee-eval-${name}-ubsan";
      value = baselineBuilt.ubsanBin;
    }
  ];
in
  builtins.listToAttrs (builtins.concatMap buildProgram programNames)

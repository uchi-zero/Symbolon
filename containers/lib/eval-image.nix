{
  pkgs,
  n2c,
  sharedLayers,
}: builtProgram: let
  inherit (pkgs) lib;

  # Single per-program layer: patched source tree + built artifacts + profile
  # JSON + image-ref stamp. With the search-profile JSON gone and program.json
  # reduced to a few rarely-changed fields, the old split into
  # source/artifacts/profile no longer earned its keep — anything that mutates
  # the program at all invalidates this layer in one go.
  #
  # The image-ref is the L6 baselineBc store path basename (content-addressed);
  # it uniquely identifies the (program, source, build-flags) tuple regardless
  # of how the image was tagged at registry-push time. eval-klee includes
  # this in the manifest as the reproducibility tuple's image identifier.
  program = pkgs.runCommand "klee-eval-${builtProgram.name}-program" {} ''
    mkdir -p $out/opt/program
    cp ${builtProgram.baselineBc}/program.bc       $out/opt/program/program.bc
    cp ${builtProgram.instrumentedBin}/program.cov $out/opt/program/program.cov
    chmod +x $out/opt/program/program.cov
    cp ${builtProgram.asanBin}/program.asan   $out/opt/program/program.asan
    cp ${builtProgram.ubsanBin}/program.ubsan $out/opt/program/program.ubsan
    chmod +x $out/opt/program/program.asan $out/opt/program/program.ubsan
    cp ${builtProgram.programJson} $out/opt/program/program.json
    cp -r ${builtProgram.patchedSrcDrv}/.      $out/opt/program/src
    chmod -R u+w $out/opt/program/src
    echo "${baseNameOf (toString builtProgram.baselineBc)}" \
      > $out/opt/program/image-ref.txt
  '';

  # /work, /out, and /tmp scratch dirs (image-resident; bind mounts override).
  # /tmp is required by LLVM coverage instrumentation (mkdtemp for profraw files).
  scratch = pkgs.runCommand "klee-eval-scratch" {} ''
    mkdir -p $out/work $out/out $out/tmp
  '';

  rootEnv = pkgs.buildEnv {
    name = "klee-eval-${builtProgram.name}-root";
    paths = [
      sharedLayers.base
      sharedLayers.klee
      sharedLayers.scripts
      program
      scratch
    ];
    pathsToLink = ["/bin" "/lib" "/share" "/etc" "/opt" "/work" "/out" "/tmp"];
    # klee-wrappers ships clang++ alongside llvmPkgs.clang; keep the LLVM
    # toolchain copy and silently drop the duplicate from klee-wrappers.
    ignoreCollisions = true;
  };
in
  n2c.nix2container.buildImage {
    name = "klee-eval-${builtProgram.name}";
    tag = "latest";

    copyToRoot = rootEnv;

    # Top of the list = most-frequently-changing. Four shared/per-program layers:
    #   program (per-program; src + artifacts + profile)
    #   scripts (this repo's entrypoints)
    #   klee    (the project's KLEE fork)
    #   base    (LLVM + gllvm + uv + runtime tools — nixpkgs-bump cadence)
    layers = [
      (n2c.nix2container.buildLayer {deps = [program];})
      (n2c.nix2container.buildLayer {deps = [sharedLayers.scripts];})
      (n2c.nix2container.buildLayer {deps = [sharedLayers.klee];})
      (n2c.nix2container.buildLayer {deps = [sharedLayers.base];})
    ];

    config = {
      Entrypoint = ["/bin/eval-klee"];
      WorkingDir = "/work";
      Env = [
        "PATH=/bin:/usr/bin"
        "SYMBOLON_PROGRAM_DIR=/opt/program"
        "SYMBOLON_BASELINE_BC=/opt/program/program.bc"
        "SSL_CERT_FILE=/etc/ssl/certs/ca-bundle.crt"
        "LLVM_COMPILER=clang"
      ];
      Labels = {
        "org.symbolon.program" = builtProgram.name;
        "org.symbolon.version" = builtProgram.version;
        "org.symbolon.image-kind" = "klee-eval";
      };
    };
  }

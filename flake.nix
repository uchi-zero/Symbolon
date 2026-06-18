{
  description = "Development environment with KLEE symbolic execution engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    # KLEE from the in-tree klee submodule (feat/symbolon branch).
    # We intentionally do NOT override klee's nixpkgs input (no follows) so that
    # klee.packages evaluates with klee's own pinned nixpkgs. When klee's nixpkgs
    # is overridden via follows, the resulting klee binary can end up with a
    # mixed glibc dependency chain (klee itself links glibc-2.40 while its minisat
    # transitive dep links glibc-2.42-61), causing a GLIBC_ABI_DT_X86_64_PLT
    # version error at runtime. The klee flake's own nixpkgs produces a consistent
    # build where all transitive deps share the same glibc minor version.
    # `git+file:./klee` consumes the in-tree klee submodule directly. Nix
    # currently warns that relative `git+file:` paths are slated for removal
    # (https://github.com/NixOS/nix/issues/12281); the warning fires at
    # lock-update time, not on every check, so it's quieter than the
    # `prev.system` deprecation it replaces. `path:./klee` doesn't work
    # for submodules since the parent repo tracks only the submodule
    # pointer, not the working-tree files.
    klee = {
      url = "git+file:./klee";
    };

    # nix2container for daemon-less, per-store-path layered OCI images
    nix2container = {
      url = "github:nlewo/nix2container";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    treefmt-nix,
    klee,
    nix2container,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [klee.overlays.default];
        };
        n2c = nix2container.packages.${system};
        # kleePkgs: klee, klee-libcxx, klee-wrappers from the klee flake's own
        # packages output. These are built in the klee flake's evaluation context
        # and have consistent glibc dependency chains, unlike the overlay-provided
        # pkgs.klee which can pick up a mismatched minisat/glibc combination.
        kleePkgs = klee.packages.${system};
        treefmtEval = treefmt-nix.lib.evalModule pkgs {
          projectRootFile = "flake.nix";
          programs.alejandra.enable = true;
          programs.mdsh.enable = true;
          programs.statix.enable = true;
          programs.shfmt.enable = true;
          programs.shellcheck.enable = true;
          programs.clang-format.enable = true;
          # Per-tool test fixtures under pass/<tool>/tests/inputs/ and
          # pass/<tool>/tests/golden/ are intentionally shaped — the goldens
          # bake in source line numbers, so reformatting the inputs shifts
          # those lines and desyncs the goldens. Keep clang-format off them.
          settings.formatter.clang-format.excludes = ["pass/*/tests/inputs/**" "pass/*/tests/golden/**"];
          settings.formatter.shellcheck.options = ["-x" "-P" "SCRIPTDIR"];
        };
      in {
        # Development shell with KLEE + uv
        devShells.default = pkgs.mkShell {
          inputsFrom = [klee.devShells.${system}.default];
          # llvmPackages_klee.clang and gllvm match the OCI image's base layer,
          # so containers/scripts/tests/test_*.sh (which compile with `clang`
          # and `gclang`) work via `nix develop --command bash <test>`.
          packages = with pkgs; [argc uv skopeo gllvm llvmPackages_klee.clang coccinelle];
        };

        # Container images live under ./containers
        packages = import ./containers {inherit pkgs n2c kleePkgs;};

        # Formatter — run with `nix fmt`
        formatter = treefmtEval.config.build.wrapper;

        # `nix flake check` runs hermetically; treefmt is intentionally NOT a
        # check here because clang-tidy needs pass/build/compile_commands.json
        # which only exists after `cmake -S pass -B pass/build`. Run formatting
        # via `nix fmt` (interactive) instead.
        checks = {
          program-builder-hello = let
            programBuilder = import ./containers/lib/program-builder.nix {inherit pkgs kleePkgs;};
            progDir = ./containers/programs/hello;
            helloHarness = import progDir {inherit pkgs;};
          in
            import ./containers/lib/program-builder.test.nix {
              inherit pkgs programBuilder helloHarness progDir;
            };
          # Regression guard for the tarball-src + no-build.patch path:
          # patchedSrcDrv `cp -r ${patchedSrc}/.` requires patchedSrc to be a
          # directory. Before the fix, programs with tarball src and no
          # build.patch left patchedSrc as the raw tarball file, so the cp
          # failed with "cp: cannot stat '.../foo.tar.gz/.': Not a directory".
          # The fix routes patchedSrc through pkgs.applyPatches uniformly
          # (empty patches list when no build.patch), so stdenv's unpackPhase
          # extracts the tarball into a directory.
          #
          # This check synthesizes a tarball from hello's source dir, feeds
          # it through programBuilder as harness.src, and asserts patchedSrcDrv
          # is a directory with the expected contents.
          program-builder-tarball-src = let
            programBuilder = import ./containers/lib/program-builder.nix {inherit pkgs kleePkgs;};
            progDir = ./containers/programs/hello;
            helloTarball = pkgs.runCommand "hello-src.tar.gz" {} ''
              cp -r ${progDir}/source hello
              tar -czf $out hello
            '';
            tarballHarness =
              (import progDir {inherit pkgs;}) // {src = helloTarball;};
            built = programBuilder {inherit progDir;} tarballHarness;
          in
            pkgs.runCommand "program-builder-test-tarball-src" {} ''
              test -d ${built.patchedSrcDrv}
              test -s ${built.patchedSrcDrv}/hello.c
              test -s ${built.patchedSrcDrv}/Makefile
              touch $out
            '';
          shared-layers = let
            sl = import ./containers/lib/shared-layers.nix {inherit pkgs kleePkgs;};
          in
            pkgs.runCommand "shared-layers-smoke" {} ''
              test -x ${sl.scripts}/bin/eval-klee
              test -x ${sl.scripts}/bin/eval-symbolon
              test -x ${sl.scripts}/bin/klee-replay-all
              test -x ${sl.scripts}/bin/cov-merge
              test -x ${sl.base}/bin/argc
              test -x ${sl.base}/bin/clang
              test -x ${sl.base}/bin/gclang
              test -x ${sl.base}/bin/patch
              test -x ${sl.base}/bin/file
              test -x ${sl.base}/bin/getconf
              test -x ${sl.base}/bin/uv
              test -x ${sl.klee}/bin/klee
              touch $out
            '';
        };
      }
    );
}

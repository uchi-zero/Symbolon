{
  pkgs,
  kleePkgs,
}: {
  progDir,
}: harness: let
  inherit (pkgs) lib;

  llvmPkgs = pkgs.llvmPackages_klee;

  # KLEE intrinsics (klee_assume, klee_make_symbolic, …) are bitcode-only:
  # KLEE resolves them during symbolic execution. The cov pass runs natively
  # (klee-replay / llvm-cov), so it must never see intrinsic calls — transformed
  # source should guard them with `#ifdef KLEE_BITCODE`. Hence the include path
  # and the macro live on the bitcode pass only.
  kleeBitcodeFlags = [
    "-isystem"
    "${kleePkgs.klee}/include"
    "-DKLEE_BITCODE"
  ];

  # UBSan instrumentation for the bitcode pass. `-fsanitize=undefined` makes
  # clang emit `__ubsan_handle_*` call sites at every UB check; KLEE's
  # `--ubsan-runtime` then links libkleeUBSan*.bca, whose handlers route into
  # klee_report_error so UB surfaces as a KLEE error state.
  #
  # Two couplings this creates — both load-bearing, do not break them:
  #   1. eval-klee MUST pass --ubsan-runtime. Instrument the bitcode but omit
  #      the runtime and KLEE sees every __ubsan_handle_* as an undefined
  #      external → spurious "failed external call" errors on every run.
  #   2. function/vptr are excluded: KLEE's UBSan runtime defines neither
  #      __ubsan_handle_function_type_mismatch nor the vptr/dynamic-type
  #      handler, so a C++ harness compiled with the full set would hit the
  #      same undefined-external pollution. The defined-handler set covers
  #      everything else (integer/shift/bounds/alignment/null/etc.).
  #
  # Bitcode only — the coverage oracle (baseInstrumentedCFlags) stays clean so
  # replayed ktests don't abort on real UB and corrupt coverage numbers.
  ubsanBitcodeFlags = [
    "-fsanitize=undefined"
    "-fno-sanitize=function,vptr"
  ];

  # Stub library of no-op intrinsic bodies. Required because the bc pass
  # produces a native binary (via gclang/system linker) from which gllvm's
  # `get-bc` then extracts the embedded bitcode. The native binary is never
  # executed — the stubs only need to satisfy the linker. The bitcode chunks
  # carry the real intrinsic call sites, which KLEE intercepts by name.
  kleeStubs =
    pkgs.runCommand "klee-intrinsic-stubs" {
      nativeBuildInputs = [llvmPkgs.clang llvmPkgs.llvm];
    } ''
      cat > stubs.c <<'EOF'
      /* Including the canonical header makes the compiler diff our stub
         signatures against KLEE's declarations — any upstream ABI change
         fails this derivation instead of corrupting linked programs. */
      #include <klee/klee.h>
      #include <stdlib.h>

      void klee_define_fixed_object(void *addr, size_t nbytes) { (void)addr; (void)nbytes; }
      void klee_make_symbolic(void *addr, size_t nbytes, const char *name) { (void)addr; (void)nbytes; (void)name; }
      int  klee_range(int begin, int end, const char *name) { (void)begin; (void)end; (void)name; return 0; }
      int  klee_int(const char *name) { (void)name; return 0; }
      void klee_silent_exit(int status) { (void)status; abort(); }
      void klee_abort(void) { abort(); }
      void klee_report_error(const char *file, int line, const char *message, const char *suffix) {
        (void)file; (void)line; (void)message; (void)suffix; abort();
      }
      size_t    klee_get_obj_size(void *ptr) { (void)ptr; return 0; }
      void      klee_print_expr(const char *msg, ...) { (void)msg; }
      uintptr_t klee_choose(uintptr_t n) { (void)n; return 0; }
      unsigned  klee_is_symbolic(uintptr_t n) { (void)n; return 0; }
      unsigned  klee_is_replay(void) { return 0; }
      void      klee_assume(uintptr_t condition) { (void)condition; }
      void      klee_warning(const char *message) { (void)message; }
      void      klee_warning_once(const char *message) { (void)message; }
      void      klee_prefer_cex(void *object, uintptr_t condition) { (void)object; (void)condition; }
      void      klee_posix_prefer_cex(void *object, uintptr_t condition) { (void)object; (void)condition; }
      void      klee_mark_global(void *object) { (void)object; }
      float     klee_get_valuef(float expr) { return expr; }
      double    klee_get_valued(double expr) { return expr; }
      long      klee_get_valuel(long expr) { return expr; }
      long long klee_get_valuell(long long expr) { return expr; }
      int32_t   klee_get_value_i32(int32_t expr) { return expr; }
      int64_t   klee_get_value_i64(int64_t expr) { return expr; }
      void      klee_check_memory_access(const void *address, size_t size) { (void)address; (void)size; }
      void      klee_set_forking(unsigned enable) { (void)enable; }
      void      klee_stack_trace(void) {}
      void      klee_print_range(const char *name, int arg) { (void)name; (void)arg; }
      void      klee_open_merge(void) {}
      void      klee_close_merge(void) {}
      int       klee_get_errno(void) { return 0; }
      EOF
      clang -O0 -fPIC -isystem ${kleePkgs.klee}/include -c stubs.c -o stubs.o
      mkdir -p $out/lib
      llvm-ar rcs $out/lib/libklee-stubs.a stubs.o
    '';

  kleeStubsLDFlags = ["-L${kleeStubs}/lib" "-lklee-stubs"];

  # Shared env vars for both build derivations.
  # NOTE: LLVM_COMPILER_PATH is intentionally omitted — gllvm misresolves llvm-link
  # when both PATH and LLVM_COMPILER_PATH are set. nativeBuildInputs put clang on PATH.
  commonEnv = {
    LLVM_COMPILER = "clang";
    NIX_CFLAGS_COMPILE = toString ["-Wno-unused-command-line-argument"];
  };

  # Build-time patch is auto-discovered: if ./build.patch exists in the
  # program's directory, it is applied to harness.src. There is no per-harness
  # `patches` field; a program that needs multiple build-time changes
  # concatenates them into one diff.
  buildPatchPath = progDir + "/build.patch";
  hasBuildPatch = builtins.pathExists buildPatchPath;

  # Always route src through applyPatches — even with an empty patches list —
  # so patchedSrc is a normalized directory derivation regardless of whether
  # harness.src is a tarball, a local directory, or already a derivation.
  # patchedSrcDrv (below, used by the image layer) `cp -r`s from patchedSrc,
  # which only works on a directory; without this normalization, tarball srcs
  # without a build.patch produce a file-typed patchedSrc and patchedSrcDrv
  # fails with "cp: cannot stat '.../foo.tar.gz/.': Not a directory". stdenv's
  # unpackPhase inside applyPatches handles tarball extraction uniformly.
  patchedSrc = pkgs.applyPatches {
    inherit (harness) src;
    patches = lib.optional hasBuildPatch buildPatchPath;
  };

  baseNativeBuildInputs = [llvmPkgs.clang llvmPkgs.llvm pkgs.gnumake];

  # The runner exports these env vars before running `harness.build.phase`:
  #   CC, CXX            — compiler to use (gclang/clang++ for bc; LLVM 16 clang for cov)
  #   CFLAGS, CXXFLAGS   — flags from `harness.bitcode.extraCFlags` / `instrumented.extraCFlags`
  #   LDFLAGS            — bc pass: -lklee-stubs; cov pass: -fprofile-instr-generate
  # Plus standard nixpkgs vars (NIX_BUILD_CORES, etc.).
  #
  # The harness phase is a bash snippet that produces `harness.build.artifact`
  # at a path inside the source root. The runner then `get-bc`s (bc pass) or
  # `cp`s (cov pass) that artifact into the derivation output.

  baseBitcodeCFlags =
    [
      "-O0"
      "-g"
      "-Xclang"
      "-disable-O0-optnone"
    ]
    ++ kleeBitcodeFlags
    ++ ubsanBitcodeFlags;

  baseInstrumentedCFlags = [
    "-O0"
    "-g"
    "-fprofile-instr-generate"
    "-fcoverage-mapping"
  ];

  bitcodeFlags = lib.concatStringsSep " " (baseBitcodeCFlags ++ (harness.bitcode.extraCFlags or []));
  # `-fsanitize=undefined` on the link line so the throwaway native binary (the
  # one get-bc extracts from) resolves the __ubsan_handle_* references against
  # compiler-rt's ubsan_standalone. Builds that compile TUs with CFLAGS then
  # link separately with LDFLAGS would otherwise fail the native link. The
  # native binary is never run; only its embedded bitcode matters.
  bitcodeLDFlags = lib.concatStringsSep " " (kleeStubsLDFlags ++ ["-fsanitize=undefined"]);
  instrumentedFlags = lib.concatStringsSep " " (baseInstrumentedCFlags ++ (harness.instrumented.extraCFlags or []));

  baselineBc = pkgs.stdenv.mkDerivation {
    pname = "${harness.name}-bc";
    inherit (harness) version;
    src = patchedSrc;
    env = commonEnv;
    nativeBuildInputs = [pkgs.gllvm] ++ baseNativeBuildInputs ++ (harness.extraNativeBuildInputs or []);
    buildInputs = harness.extraBuildInputs or [];
    # Harnesses own configure entirely (their build.phase runs ./configure
    # with the flag set they want). Letting stdenv auto-configure first with
    # generic flags either wastes a configure pass or — for projects like
    # curl that demand specific opt-in flags — fails outright.
    dontConfigure = true;
    buildPhase = ''
      runHook preBuild
      export CC=gclang
      export CXX=gclang++
      export CFLAGS=${lib.escapeShellArg bitcodeFlags}
      export CXXFLAGS=${lib.escapeShellArg bitcodeFlags}
      export LDFLAGS=${lib.escapeShellArg bitcodeLDFlags}
      ${harness.build.phase}
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out
      get-bc -o $out/program.bc ${harness.build.artifact}
      runHook postInstall
    '';
  };

  instrumentedBin = pkgs.stdenv.mkDerivation {
    pname = "${harness.name}-cov";
    inherit (harness) version;
    src = patchedSrc;
    env = commonEnv;
    nativeBuildInputs = baseNativeBuildInputs ++ (harness.extraNativeBuildInputs or []);
    buildInputs = harness.extraBuildInputs or [];
    dontConfigure = true;
    buildPhase = ''
      runHook preBuild
      # Pin to LLVM 16 clang so coverage-mapping format matches llvm-cov / llvm-profdata.
      export CC=${llvmPkgs.clang}/bin/clang
      export CXX=${llvmPkgs.clang}/bin/clang++
      export CFLAGS=${lib.escapeShellArg instrumentedFlags}
      export CXXFLAGS=${lib.escapeShellArg instrumentedFlags}
      export LDFLAGS="-fprofile-instr-generate"
      ${harness.build.phase}
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out
      cp ${harness.build.artifact} $out/program.cov
      chmod +x $out/program.cov
      runHook postInstall
    '';
  };

  # ASan / UBSan native builds. Same derivation shape as instrumentedBin (built
  # with the same KLEE LLVM-16 clang wrapper, so klee-replay drives them exactly
  # like program.cov), with the coverage flags swapped for sanitizer flags.
  # Built from baseline patchedSrc — the unmodified "original" program. Produces
  # $out/program.<suffix> (program.asan / program.ubsan).
  mkSanitizerBin = sanitizer: suffix:
    pkgs.stdenv.mkDerivation {
      pname = "${harness.name}-${suffix}";
      inherit (harness) version;
      src = patchedSrc;
      env = commonEnv;
      nativeBuildInputs = baseNativeBuildInputs ++ (harness.extraNativeBuildInputs or []);
      buildInputs = harness.extraBuildInputs or [];
      dontConfigure = true;
      buildPhase = ''
        runHook preBuild
        export CC=${llvmPkgs.clang}/bin/clang
        export CXX=${llvmPkgs.clang}/bin/clang++
        export CFLAGS=${lib.escapeShellArg (lib.concatStringsSep " " (["-O0" "-g" "-fno-omit-frame-pointer" "-fsanitize=${sanitizer}"] ++ (harness.instrumented.extraCFlags or [])))}
        export CXXFLAGS="$CFLAGS"
        export LDFLAGS="-fsanitize=${sanitizer}"
        ${harness.build.phase}
        runHook postBuild
      '';
      installPhase = ''
        runHook preInstall
        mkdir -p $out
        cp ${harness.build.artifact} $out/program.${suffix}
        chmod +x $out/program.${suffix}
        runHook postInstall
      '';
    };
  asanBin = mkSanitizerBin "address" "asan";
  ubsanBin = mkSanitizerBin "undefined" "ubsan";

  # Program profile JSON (baked). KLEE is always invoked with --posix-runtime
  # and --libc=uclibc, so those aren't recorded; the only varying knobs are:
  #   with-libcxx  — pass --libcxx for C++ programs
  #   sym-env      — args appended after the bitcode (e.g. --sym-args, --sym-stdin)
  programJson = pkgs.writeText "program.json" (builtins.toJSON {
    inherit (harness) name version;
    with-libcxx = harness.withLibcxx;
    sym-env = harness.symEnv;
  });

  # Post-build.patch source tree as a standalone derivation, so the image
  # layer can copy it under /opt/program/src. patchedSrc itself may be either
  # the raw source (no build.patch) or an applyPatches output; wrapping in
  # runCommand normalizes both into a derivation we can ${interpolate}.
  patchedSrcDrv = pkgs.runCommand "${harness.name}-src" {} ''
    mkdir -p $out
    cp -r ${patchedSrc}/. $out/
    chmod -R u+w $out
  '';

in {
  inherit
    baselineBc
    instrumentedBin
    asanBin
    ubsanBin
    programJson
    patchedSrcDrv
    ;
  inherit (harness) name version;
}

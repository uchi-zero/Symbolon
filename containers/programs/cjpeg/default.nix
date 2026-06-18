{pkgs, ...}: {
  name = "cjpeg";
  version = "3.1.4.1";
  notes = "";

  src = pkgs.fetchurl {
    url = "https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/3.1.4.1/libjpeg-turbo-3.1.4.1.tar.gz";
    # Verified with: nix-prefetch-url <url> then `nix hash to-sri --type sha256 <hex>`.
    hash = "sha256-7K6ACOLMmt4vLBu51ebU+3PnxDOGagVr2CmAdBVxoCI=";
  };

  # libjpeg-turbo 3.x is CMake-only; pull cmake into the build env.
  extraNativeBuildInputs = [pkgs.cmake];

  build = {
    # cjpeg-static is the variant that links libjpeg statically —
    # the only configuration KLEE can usefully execute.
    artifact = "build/cjpeg-static";
    phase = ''
      # A transformation that inserts klee_assume / klee_range (e.g.
      # ppm-comment-pin.patch) makes the .o files reference those symbols, so
      # -lklee-stubs (set in LDFLAGS by program-builder.nix) must land AFTER
      # the objects in the link line. CMAKE_EXE_LINKER_FLAGS is emitted before
      # the objects, so move the stubs lib into CMAKE_C_STANDARD_LIBRARIES,
      # which CMake appends at the very end of the link command. Harmless when
      # no transformation is applied (no references → the lib is simply unused).
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+klee-intrinsic-stubs[^ ]*|-lklee-stubs)' | tr '\n' ' '; } || true)
      link_flags=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')

      # WITH_SIMD off — SIMD codepaths use NASM assembly that bypasses
      # gllvm. ENABLE_SHARED off so we don't waste a build pass on the
      # .so. WITH_TURBOJPEG / WITH_JAVA off — extra surface we don't need.
      cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_C_FLAGS="$CFLAGS" \
        -DCMAKE_EXE_LINKER_FLAGS="$link_flags" \
        -DCMAKE_C_STANDARD_LIBRARIES="$stubs_lib_flags" \
        -DENABLE_SHARED=OFF \
        -DENABLE_STATIC=ON \
        -DWITH_SIMD=OFF \
        -DWITH_TURBOJPEG=OFF \
        -DWITH_JAVA=OFF
      cmake --build build --target cjpeg-static -j$NIX_BUILD_CORES
    '';
  };

  withLibcxx = false;

  # --sym-args 0 2 4 : 0–2 symbolic CLI options each ≤4 bytes
  # A                : literal positional → matches KLEE's symbolic-file name "A"
  # --sym-files 1 100: 1 symbolic input file of 100 bytes (the JPEG-input candidate)
  symEnv = [
    "--sym-args"
    "0"
    "2"
    "4"
    "A"
    "--sym-files"
    "1"
    "100"
  ];
}

{pkgs, ...}: {
  name = "jasper";
  version = "4.2.9";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://github.com/jasper-software/jasper/archive/refs/tags/version-4.2.9.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/jasper-software/jasper/archive/refs/tags/version-4.2.9.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-sOWva1TCdLlnDH4y3b9sgC2IyJYGLXYCZ2ld0KpwFP8=";
  };

  # Jasper is the first cmake/ninja program in the tree.
  extraNativeBuildInputs = [pkgs.cmake pkgs.ninja];

  build = {
    artifact = "bld/src/app/jasper";
    phase = ''
      # Force-feed cmake the env CC/CFLAGS/LDFLAGS that program-builder
      # exports — relying on cmake's auto-pickup is fragile across builds.
      # Strip every optional codec/dep so the binary doesn't try to link
      # against libjpeg/libheif/opengl, which we don't ship under klee-uclibc.
      # Binary dir is `bld/` (not `build/`) because the source tree already
      # contains a build/ directory of helper scripts.
      cmake -S . -B bld -G Ninja \
        -DALLOW_IN_SOURCE_BUILD=ON \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_C_FLAGS="$CFLAGS" \
        -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
        -DBUILD_SHARED_LIBS=OFF \
        -DJAS_ENABLE_SHARED=OFF \
        -DJAS_ENABLE_LIBJPEG=OFF \
        -DJAS_ENABLE_LIBHEIF=OFF \
        -DJAS_ENABLE_OPENGL=OFF \
        -DJAS_ENABLE_DOC=OFF \
        -DJAS_ENABLE_LATEX=OFF \
        -DJAS_ENABLE_MULTITHREADING_SUPPORT=OFF \
        -DJAS_ENABLE_PROGRAMS=ON
      cmake --build bld --target jasper -j$NIX_BUILD_CORES
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--input"
    "A"
    "--output"
    "B"
    "--input-format"
    "--sym-arg"
    "3"
    "--output-format"
    "--sym-arg"
    "3"
    "--sym-args"
    "0"
    "3"
    "15"
    "--sym-files"
    "2"
    "300"
    "--save-all-writes"
  ];
}

{pkgs, ...}: {
  name = "flvmeta";
  version = "1.2.2";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://github.com/noirotm/flvmeta/archive/refs/tags/v1.2.2.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/noirotm/flvmeta/archive/refs/tags/v1.2.2.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-WTceKGFo1uXEZH01dcAby7MBR8SRbraeEPOM28HFVG0=";
  };

  extraNativeBuildInputs = [pkgs.cmake pkgs.ninja];

  build = {
    artifact = "build/src/flvmeta";
    phase = ''
      # flvmeta's CMakeLists declares cmake_minimum_required < 3.5, which
      # newer CMake refuses outright; the policy override is the documented
      # workaround.
      # CMake places EXE_LINKER_FLAGS before object files. Repeat LDFLAGS in
      # C_STANDARD_LIBRARIES so transformed objects can resolve KLEE stubs.
      cmake -S . -B build -G Ninja \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_C_FLAGS="$CFLAGS" \
        -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
        -DCMAKE_C_STANDARD_LIBRARIES="$LDFLAGS" \
        -DBUILD_SHARED_LIBS=OFF
      cmake --build build --target flvmeta -j$NIX_BUILD_CORES
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-arg"
    "2"
    "--sym-args"
    "0"
    "2"
    "6"
    "A"
    "--sym-files"
    "1"
    "300"
    "--save-all-writes"
  ];
}

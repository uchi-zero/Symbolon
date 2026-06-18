{pkgs, ...}: {
  name = "libxml2";
  version = "2.15.3";
  notes = "from paper cgs";

  src = pkgs.fetchurl {
    url = "https://github.com/GNOME/libxml2/archive/refs/tags/v2.15.3.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/GNOME/libxml2/archive/refs/tags/v2.15.3.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri --from nix32 <hash>`.
    hash = "sha256-XGBgJ3FzJwNWw/HDIaZAq2Kb2rxeW6kJW5ngB1m6DDk=";
  };

  # The GitHub source archive omits the autotools-generated `configure`
  # script, so autotools would need autoreconf. libxml2 ships CMake
  # in-tree and it works against this tarball without regeneration.
  extraNativeBuildInputs = [pkgs.cmake];

  build = {
    artifact = "build/xmllint";
    phase = ''
      # Turn off every optional feature that pulls in an external library
      # (iconv, icu, lzma, zlib, threads, dlopen modules, http) so the
      # extracted bitcode is self-contained. Parser-side features
      # (XPath, XInclude, schemas, HTML, regexps, …) stay at upstream
      # defaults — those are the code paths KLEE explores.
      cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_C_FLAGS="$CFLAGS" \
        -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
        -DBUILD_SHARED_LIBS=OFF \
        -DLIBXML2_WITH_PROGRAMS=ON \
        -DLIBXML2_WITH_TESTS=OFF \
        -DLIBXML2_WITH_PYTHON=OFF \
        -DLIBXML2_WITH_DOCS=OFF \
        -DLIBXML2_WITH_ICONV=OFF \
        -DLIBXML2_WITH_ICU=OFF \
        -DLIBXML2_WITH_LZMA=OFF \
        -DLIBXML2_WITH_ZLIB=OFF \
        -DLIBXML2_WITH_THREADS=OFF \
        -DLIBXML2_WITH_MODULES=OFF \
        -DLIBXML2_WITH_HTTP=OFF
      cmake --build build --target xmllint -j$NIX_BUILD_CORES
    '';
  };

  withLibcxx = false;

  # A literal positional → KLEE's symbolic file is named "A" by convention.
  # --sym-files 1 40 : 1 symbolic input file of 40 bytes (the XML candidate).
  symEnv = [
    "A"
    "--sym-files"
    "1"
    "40"
  ];
}

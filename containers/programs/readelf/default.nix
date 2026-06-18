{pkgs, ...}: {
  name = "readelf";
  version = "2.46.0";
  notes = "from paper empc, learch, cgs";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-hgj+RKt95kX2rQqJgxO3UziEJJDWCa24XJ+ygnw3avI=";
  };

  # binutils' all-binutils target runs makeinfo for the binutils/.info docs.
  extraNativeBuildInputs = [pkgs.texinfo];

  build = {
    artifact = "binutils/readelf";
    phase = ''
      ./configure \
        --disable-werror \
        --disable-nls \
        --disable-gas \
        --disable-ld \
        --disable-gold \
        --disable-gprof \
        --disable-gprofng \
        --disable-shared \
        --enable-static \
        --disable-plugins \
        --disable-readline \
        --disable-multilib \
        --without-zstd
      make -j$NIX_BUILD_CORES all-binutils
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "-a"
    "A"
    "--sym-files"
    "1"
    "100"
  ];
}

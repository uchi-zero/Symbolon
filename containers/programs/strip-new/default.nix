{pkgs, ...}: {
  name = "strip-new";
  version = "2.46.0";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-hgj+RKt95kX2rQqJgxO3UziEJJDWCa24XJ+ygnw3avI=";
  };

  # binutils' all-binutils target runs makeinfo for the binutils/.info docs.
  extraNativeBuildInputs = [pkgs.texinfo];

  build = {
    artifact = "binutils/strip-new";
    phase = ''
      # Disable every binutils sibling so the supermake only walks the deps
      # strip-new actually needs (bfd, libiberty, opcodes, zlib, libsframe,
      # libctf, binutils). --disable-werror is needed because gllvm/clang
      # surfaces warnings the gcc-tuned tree treats as errors.
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
      # all-binutils descends through binutils' deps and builds binutils/,
      # which produces the in-tree strip-new (renamed to `strip` on install).
      make -j$NIX_BUILD_CORES all-binutils
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "2"
    "8"
    "A"
    "--sym-files"
    "1"
    "100"
  ];
}

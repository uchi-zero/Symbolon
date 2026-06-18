{pkgs, ...}: {
  name = "objcopy";
  version = "2.46.0";
  notes = "from paper learch, cgs";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-hgj+RKt95kX2rQqJgxO3UziEJJDWCa24XJ+ygnw3avI=";
  };

  # binutils' all-binutils target runs makeinfo for the binutils/.info docs.
  # bison + flex are needed because `make all-binutils` regenerates the
  # bison/lex-generated parsers (sysinfo.c, arparse.c, deflex.c, …) whenever
  # eval-transformations' `cp -r` of the source tree gives a generated .c and
  # its .y/.l source near-equal mtimes. Without them on the runtime PATH the
  # rebuild dies with "bison: not found" / Error 127 (the flaky build the
  # readelf transformations README documents). They never change program.bc:
  # the regenerated parsers are byte-identical to the shipped ones.
  extraNativeBuildInputs = [pkgs.texinfo pkgs.bison pkgs.flex pkgs.m4];

  build = {
    artifact = "binutils/objcopy";
    phase = ''
      # Disable every binutils sibling so the supermake only walks the deps
      # objcopy actually needs (bfd, libiberty, opcodes, zlib, libsframe,
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
      make -j$NIX_BUILD_CORES all-binutils
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "3"
    "20"
    "A"
    "--sym-files"
    "1"
    "100"
  ];
}

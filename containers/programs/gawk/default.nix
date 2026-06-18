{pkgs, ...}: {
  name = "gawk";
  version = "5.4.0";
  notes = "from paper learch, cgs";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/gawk/gawk-5.4.0.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/gawk/gawk-5.4.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-31dW1QdyISqOPybZAxB+zjdzxAN8ap4KWcKgqNcynw0=";
  };

  build = {
    artifact = "gawk";
    phase = ''
      # A transformation that edits a source file (e.g. the bison-generated
      # awkgram.c) makes it newer than the autotools-generated tree. gawk has
      # no AM_MAINTAINER_MODE, so make then trips its auto-remake rules and
      # tries to run aclocal/automake/bison -- none of which exist in the eval
      # container, giving "make[1]: *** [aclocal.m4] Error 127". (Even an
      # unpatched eval-transformations run hits this: its `cp -r` of the source
      # tree skews mtimes.) Normalize mtimes so the autotools-generated files
      # (aclocal.m4, configure, config.h.in, Makefile.in) and the bison-
      # generated parser sources (awkgram.c, command.c) stay up to date and no
      # regen is attempted. A no-op for the unpatched baseline (bitcode is
      # byte-identical); it only fixes the broken auto-remake trigger.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      touch -d '2020-01-02 00:00:00' aclocal.m4 configure config.h.in \
            Makefile.in */Makefile.in 2>/dev/null || true
      touch -d '2020-01-03 00:00:00' awkgram.c command.c 2>/dev/null || true

      # --without-mpfr drops the GMP/MPFR arbitrary-precision arithmetic dep;
      # --without-readline keeps the link clean under klee-uclibc. Build the
      # support/ helper lib first (libsupport.a is a hard dep of the gawk
      # binary), then gawk itself; skip extras/doc/awklib/po/test to avoid
      # texinfo and shell-test machinery.
      ./configure \
        --disable-nls \
        --disable-dependency-tracking \
        --without-mpfr \
        --without-readline
      make -j$NIX_BUILD_CORES -C support
      make -j$NIX_BUILD_CORES gawk
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "-f"
    "A"
    "B"
    "--sym-files"
    "2"
    "50"
  ];
}

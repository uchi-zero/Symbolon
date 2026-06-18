{pkgs, ...}: {
  name = "find";
  version = "4.10.0";
  notes = "from paper learch";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/findutils/findutils-4.10.0.tar.xz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/findutils/findutils-4.10.0.tar.xz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-E4fgtn/yR9Kr3pmPkN+/cMFJE5Glnd/suK5ph4nwpPU=";
  };

  build = {
    artifact = "find/find";
    phase = ''
      # Normalize mtimes before configure. At nix-build time the source comes
      # from applyPatches with the tarball's dependency-correct mtimes, but
      # eval-transformations rebuilds from a `cp -r` of the baked tree (all
      # mtimes reset), which makes configure.ac look newer than aclocal.m4 and
      # trips AM_MAINTAINER_MODE into regenerating aclocal/configure (needs
      # aclocal-1.16, absent at runtime). Touch everything to one timestamp,
      # then bump the autotools-generated files just past it so make treats
      # them as up to date. (Same idiom as the bison/bash harnesses.)
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      find . \( -name configure -o -name aclocal.m4 -o -name config.h.in \
             -o -name 'Makefile.in' -o -name '*.info' \) \
        -exec touch -h -d '2020-01-02 00:00:00' {} + 2>/dev/null || true
      ./configure --disable-nls --disable-dependency-tracking
      # A transformation that inserts klee_range / klee_assume (e.g.
      # find-arg-menu-pin) makes ftsfind.o reference those symbols, satisfied
      # by -lklee-stubs (put in LDFLAGS by program-builder.nix). find's link
      # rule places LDFLAGS *before* the objects, so the stubs lib is dropped
      # and the link fails with undefined references. Move it to LIBS, which
      # automake appends after the objects. Harmless with no transformation
      # (no references → -Wl,--as-needed drops it) and for the cov build
      # (its LDFLAGS carry no stubs). Same idiom as the make harness.
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      # Build order matters: gl/lib (gnulib stubs) → lib (findutils helpers,
      # libfind.a) → find. Skip the top-level `all` target to avoid the
      # texinfo dep on doc/find.info.
      make -j$NIX_BUILD_CORES -C gl/lib
      make -j$NIX_BUILD_CORES -C lib
      make -j$NIX_BUILD_CORES -C find LIBS="$stubs_lib_flags"
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "3"
    "10"
    "--sym-files"
    "1"
    "40"
    "--sym-stdin"
    "40"
  ];

}

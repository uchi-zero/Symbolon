{pkgs, ...}: {
  name = "diffutils";
  version = "3.12";
  notes = "from paper learch";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/diffutils/diffutils-3.12.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/diffutils/diffutils-3.12.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-W+GBsn7Diq0kUAgGYaZOShdSuym31QUr8KAqcPYj+bI=";
  };

  build = {
    artifact = "src/diff";
    phase = ''
      # A transformation that edits any src/*.c (e.g. input-prefix-pin in
      # diff.c) makes that source newer than the staged tree's generated
      # autotools files. diffutils ships AM_MAINTAINER_MODE *enabled*, so make
      # then tries to re-run aclocal/autoheader/automake -- none of which exist
      # in the eval container -- giving "aclocal-1.17 / autoheader: command not
      # found" -> "[config.hin] Error 127". Normalize mtimes (cf. bc/strip-new)
      # so the generated files stay newer than their m4/.ac sources in
      # dependency order; only the patched .c is newer than its .o, forcing
      # just the recompile we want. A no-op for the unpatched baseline.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      touch -d '2020-01-02 00:00:00' aclocal.m4 2>/dev/null || true
      touch -d '2020-01-03 00:00:00' configure config.hin config.h.in \
            lib/config.hin lib/config.h.in 2>/dev/null || true
      touch -d '2020-01-04 00:00:00' Makefile.in */Makefile.in 2>/dev/null || true

      # The src/Makefile diff link rule is
      #   $(LINK) $(diff_OBJECTS) $(diff_LDADD) $(LIBS)
      # with -lklee-stubs arriving via LDFLAGS (inside $(LINK)) -- i.e. BEFORE
      # the objects. A patch that inserts klee_assume/klee_make_symbolic leaves
      # undefined references in diff.o, which -Wl,--as-needed then drops because
      # the stub lib preceded the objects -> "undefined reference to
      # klee_assume". Pull the stub lib out of LDFLAGS and pass it via LIBS so
      # it lands AFTER the objects (cf. the bc/make harnesses). Harmless for the
      # baseline: with no references, --as-needed drops the stub lib anyway.
      stubs=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')

      ./configure --disable-nls --disable-dependency-tracking
      # Same pattern as sed/bc: build the gnulib helper lib first so src/diff
      # can link against libdiffutils.a, and skip the top-level `all` target
      # to avoid the texinfo dep that builds doc/diffutils.info.
      make -j$NIX_BUILD_CORES -C lib LIBS="$stubs"
      make -j$NIX_BUILD_CORES -C src diff LIBS="$stubs"
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "2"
    "2"
    "A"
    "B"
    "--sym-files"
    "2"
    "50"
  ];
}

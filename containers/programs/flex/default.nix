{pkgs, ...}: {
  name = "flex";
  version = "2.6.4";
  notes = "";

  src = pkgs.fetchurl {
    url = "https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-6HquAyvwfCb4WsDtMlCZjDdiHZX4vXSLMfFbM8Re6ZU=";
  };

  # flex bootstraps itself (a stage1flex builds the final flex's lexer), and
  # m4 is invoked at runtime by the generated scanners during the build.
  extraNativeBuildInputs = [pkgs.m4];

  build = {
    artifact = "src/flex";
    phase = ''
      # eval-transformations copies the source tree before patching it, which
      # does not preserve the tarball's mtimes. flex has no AM_MAINTAINER_MODE,
      # so a generated file that ends up older than its source trips autotools'
      # auto-remake rules into running aclocal/autoheader/automake (and the
      # flex/bison .l/.y regen) -- none of which exist in the eval container,
      # giving "make[1]: *** [aclocal.m4] Error 127". Normalize mtimes so the
      # shipped generated files (configure, Makefile.in, and the bootstrapped
      # scan.c/parse.c) stay authoritative. A no-op for the unpatched baseline.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      find . \( -name configure -o -name aclocal.m4 -o -name config.h.in \
                -o -name Makefile.in -o -name scan.c -o -name parse.c \
                -o -name parse.h -o -name skel.c \) \
        -exec touch -h -d '2020-01-02 00:00:00' {} + 2>/dev/null || true

      # flex's link rule places $(LDFLAGS) before the object files, so a
      # transformation that inserts klee_assume / klee_range / klee_is_symbolic
      # leaves those references in the objects and the stub library (carried in
      # LDFLAGS) is searched too early -> "undefined reference to klee_assume".
      # Move -lklee-stubs into LIBS, which the link rule emits AFTER the objects.
      # Pass it through the configure-time LIBS env var (autoconf prepends its
      # own detected libs -- e.g. -lm from AC_CHECK_LIB(m,log10) -- and keeps the
      # env LIBS at the tail), so the final link is `... objects ... -lm
      # -lklee-stubs`. This preserves -lm (the cov build needs log10/ceil) while
      # satisfying klee references. Harmless for the baseline: with no
      # references, -Wl,--as-needed drops the stub lib. Mirrors bc / bison / make.
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      export LIBS="$stubs_lib_flags''${LIBS:+ $LIBS}"

      # --disable-shared keeps src/flex a real binary instead of a libtool
      # wrapper script. Build only src/; doc/ would pull in texinfo via its
      # info_TEXINFOS, and we don't need the manuals.
      ./configure \
        --disable-shared \
        --enable-static \
        --disable-nls \
        --disable-dependency-tracking
      make -j$NIX_BUILD_CORES -C src
    '';
  };

  # The FLEX_EXIT macro switches to plain exit() only in the KLEE bitcode
  # build (see build.patch). The instrumented build keeps the
  # original longjmp path so atexit cleanup runs from main()'s stack frame,
  # which avoids corrupting LLVM's profile counters during klee-replay.
  bitcode.extraCFlags = ["-Wno-implicit-function-declaration" "-DSYMBOLON_KLEE_BITCODE=1"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "5"
    "12"
    "A"
    "--sym-files"
    "1"
    "300"
  ];
}

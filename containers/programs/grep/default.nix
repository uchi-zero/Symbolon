{pkgs, ...}: {
  name = "grep";
  version = "3.12";
  notes = "from paper learch, cgs";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/grep/grep-3.12.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/grep/grep-3.12.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-ut2lRt/Eudl+mS4sNfO1x/IFIv/L4vAboenNy+dkTNw=";
  };

  build = {
    artifact = "src/grep";
    phase = ''
      # autotools auto-remake guard. The baseline bitcode builds under Nix,
      # where the realized source tree has uniform store mtimes so nothing
      # looks stale. But eval-transformations stages the tree with `cp -r`
      # (mtimes = copy time, in arbitrary order), which can leave a source
      # like configure.ac / m4/*.m4 / Makefile.am newer than its generated
      # output (aclocal.m4, configure, Makefile.in). grep has no
      # AM_MAINTAINER_MODE, so make then tries to re-run aclocal/automake/
      # autoconf -- absent in the eval container -> "[aclocal.m4] Error 127".
      # Normalize mtimes so every generated autotools file is newer than its
      # inputs. grep ships no flex/bison sources, so there is nothing else to
      # regenerate; this is a no-op for the unpatched baseline (byte-identical
      # bitcode) and only ever prevents the spurious remake.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      touch -d '2020-01-02 00:00:00' aclocal.m4 configure config.hin config.h.in \
            Makefile.in */Makefile.in lib/Makefile.in src/Makefile.in 2>/dev/null || true

      # stub link order. grep's link rule (src/Makefile: LINK = ... $(LDFLAGS)
      # -o $@ <objects> $(LIBS)) emits $(LDFLAGS) BEFORE the objects but $(LIBS)
      # AFTER them. A transformation that inserts klee_assume / klee_make_symbolic
      # leaves those references in grep.o, so the stub library must come AFTER the
      # objects or the link fails with "undefined reference to klee_assume". Move
      # -lklee-stubs out of LDFLAGS and feed it via LIBS (cf. the bc/make
      # harnesses). Harmless for the unpatched baseline: with no klee references,
      # -Wl,--as-needed drops the stub lib and the bitcode is byte-identical.
      stubs=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')

      ./configure --disable-nls --disable-dependency-tracking
      # Same lib-then-src pattern as sed/diffutils: build the gnulib helper
      # lib first, then src/grep alone, skipping the top-level `all` target
      # so we don't pull in the texinfo dep for doc/grep.info.
      make -j$NIX_BUILD_CORES -C lib LIBS="$stubs"
      make -j$NIX_BUILD_CORES -C src grep LIBS="$stubs"
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
    "--sym-arg"
    "10"
    "A"
    "--sym-files"
    "1"
    "50"
  ];
}

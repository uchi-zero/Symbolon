{pkgs, ...}: {
  name = "sed";
  version = "4.10";
  notes = "from paper cgs";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/sed/sed-4.10.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/sed/sed-4.10.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-TRef+vkuxNzsVB98Ayvhw7mhhW9JcK25WlBSIXAvUnc=";
  };

  # gnulib's BUILT_SOURCES under lib/ are tied to the `all` target, which
  # also builds doc/sed.info via texinfo. Same pattern as the bison harness.
  extraNativeBuildInputs = [pkgs.texinfo];

  build = {
    artifact = "sed/sed";
    phase = ''
      # A transformation that edits any sed/*.c (e.g. the script menu-pin in
      # sed.c) makes that source newer than the staged tree's generated
      # autotools files. sed ships AM_INIT_AUTOMAKE without
      # AM_MAINTAINER_MODE(disabled), so make then tries to re-run aclocal/
      # automake/autoconf -- none of which exist in the eval container -- giving
      # "aclocal-1.18: command not found" -> "[aclocal.m4] Error 127". Normalize
      # mtimes (cf. bc/diffutils/strip-new) so the generated files stay newer
      # than their m4/.ac sources in dependency order; the patched .c is built
      # fresh anyway (no pre-existing .o), so this forces no spurious regen. A
      # no-op for the unpatched baseline.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      touch -d '2020-01-02 00:00:00' aclocal.m4 2>/dev/null || true
      touch -d '2020-01-03 00:00:00' configure config_h.in 2>/dev/null || true
      touch -d '2020-01-04 00:00:00' Makefile.in */Makefile.in 2>/dev/null || true

      # sed's link rule (generated Makefile) is
      #   $(sed_sed_LINK) $(sed_sed_OBJECTS) $(sed_sed_LDADD) $(LIBS)
      # with -lklee-stubs arriving via LDFLAGS (inside $(sed_sed_LINK)) -- i.e.
      # BEFORE the objects. A patch that inserts klee_assume / klee_range leaves
      # undefined references in sed.o, which -Wl,--as-needed then drops because
      # the stub lib preceded the objects -> "undefined reference to
      # klee_assume". Pull the stub lib out of LDFLAGS and pass it via LIBS so it
      # lands AFTER the objects (cf. the bc/diffutils harnesses). Harmless for
      # the baseline: with no references, --as-needed drops the stub lib anyway.
      stubs=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')

      ./configure --disable-nls --disable-dependency-tracking
      # Build the sed binary ALONE -- NOT the `all` target. `all` also builds
      # doc/sed.1 via help2man, which runs `sed/sed --help` to scrape usage;
      # under KLEE_BITCODE the binary calls klee_range/klee_assume at startup, so
      # running it natively aborts and help2man fails ("[doc/sed.1] Error 255").
      # Targeting sed/sed directly skips doc/ entirely (same "skip doc to dodge
      # the doc tool" pattern as the bc/diffutils harnesses). The Makefile's
      #   $(sed_sed_OBJECTS): $(BUILT_SOURCES)
      # dependency pulls in gnulib's generated headers first automatically, so
      # this single target is self-contained. A no-op difference for the
      # baseline, which links the same sed/sed object either way.
      make -j$NIX_BUILD_CORES sed/sed LIBS="$stubs"
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-arg"
    "10"
    "--sym-args"
    "0"
    "2"
    "2"
    "--sym-files"
    "1"
    "8"
    "--sym-stdin"
    "8"
  ];
}

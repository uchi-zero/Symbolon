{pkgs, ...}: {
  name = "bash";
  version = "5.3";
  notes = "";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/bash/bash-5.3.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/bash/bash-5.3.tar.gz
    # then `nix hash to-sri --type sha256 <hex>` to get the SRI form below.
    hash = "sha256-DVzYaWX4aaJs9k9Lcb57lvkKO6iz104n6OnZ1VUPMbo=";
  };

  build = {
    artifact = "bash";
    phase = ''
      # Defeat the autotools/yacc maintainer rules before building. The nix
      # baseline build unpacks the tarball with its shipped mtimes (generated
      # files already newer than their sources), so `make` never tries to
      # regenerate them. eval-transformations, however, stages the tree with
      # `cp -r` (and a patch), which normalizes mtimes — leaving `configure`
      # no newer than `configure.ac` and `y.tab.c`/parser-built no newer than
      # `parse.y`. That fires `cd . && autoconf` and `$(YACC) -d parse.y`,
      # but the eval runtime dev shell has neither autoconf nor bison, so the
      # build dies with "autoconf: not found". Touch the generated artifacts
      # strictly newer than their sources (dependency-ordered) so the rules
      # stay satisfied. Harmless on the tarball path (just re-asserts order).
      touch -t 202001010000 configure.ac aclocal.m4 config.h.in \
                            config-top.h config-bot.h parse.y \
                            Makefile.in pathnames.h.in buildconf.h.in
      touch -t 202001010001 configure
      touch -t 202001010002 y.tab.c
      touch -t 202001010003 y.tab.h
      touch -t 202001010004 parser-built
      ./configure \
        --without-bash-malloc \
        --disable-nls \
        --disable-readline \
        --disable-history

      # Static-archive link ordering for klee-stubs. program-builder.nix puts
      # `-L<klee-intrinsic-stubs> -lklee-stubs` in LDFLAGS, and bash's link rule
      # (Makefile:635) emits `$(LDFLAGS)` BEFORE `$(OBJECTS)`:
      #   $(CC) ... $(LDFLAGS) -o $(Program) $(OBJECTS) $(LIBS)
      # With a static libklee-stubs.a placed first, ld processes and discards it
      # before any object references its symbols, so a transformation that emits
      # klee_assume / klee_range (e.g. an input-menu pin) fails to link with
      # "undefined reference to klee_range". The stub library must come AFTER the
      # objects. bc/make pass `make LIBS=<stubs>`, but bash's `LIBS` is built from
      # make variables ($(BUILTINS_LIB) $(LIBRARIES) @LIBS@), so overriding it on
      # the command line would drop bash's own -lbuiltins/-lglob/-lsh/-ldl. Instead
      # append the stub flags to the end of the link recipe, after $(LIBS). Harmless
      # on the baseline (no klee references → ld pulls nothing from the archive) and
      # it does not affect get-bc's bitcode extraction, so program.bc is unchanged.
      stub_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+klee-intrinsic-stubs[^ ]*|-lklee-stubs)' | tr '\n' ' '; } || true)
      if [ -n "$stub_flags" ]; then
        sed -i -E "s|(-o \\\$\\(Program\\) \\\$\\(OBJECTS\\) \\\$\\(LIBS\\))|\\1 $stub_flags|" Makefile
      fi

      make -j$NIX_BUILD_CORES bash
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
    "--sym-stdin"
    "100"
  ];

}

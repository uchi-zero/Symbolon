{pkgs, ...}: {
  name = "make";
  version = "4.4";
  notes = "from paper empc, learch, cgs";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/make/make-4.4.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/make/make-4.4.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-WB9NToctp0s5Qch0IViYp9NYAvA3Mr3M7h1KeXkQXRg=";
  };

  build = {
    artifact = "make";
    phase = ''
      # A transformation that edits any src/*.c (e.g. input-prefix-pin in
      # main.c) makes that source newer than the staged tree's generated
      # autotools files. make 4.4 has NO AM_MAINTAINER_MODE, so the
      # `$(ACLOCAL_M4): $(am__aclocal_m4_deps)` rule always fires once an m4/.ac
      # dep looks newer than aclocal.m4 -- and the eval container has no
      # aclocal-1.16 -> "[aclocal.m4] Error 127". The baked src arrives with all
      # mtimes flattened to epoch-1, and eval-transformations' plain `cp -r`
      # reorders them, so the regen rule fires even for the unpatched baseline.
      # Normalize mtimes (cf. diffutils) so the generated files stay newer than
      # their m4/.ac sources in dependency order; only the patched .c is newer
      # than its .o, forcing just the recompile we want. A no-op for the
      # unpatched baseline.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      touch -d '2020-01-02 00:00:00' aclocal.m4 2>/dev/null || true
      touch -d '2020-01-03 00:00:00' configure src/config.h.in 2>/dev/null || true
      touch -d '2020-01-04 00:00:00' Makefile.in */Makefile.in 2>/dev/null || true

      ./configure --disable-nls --disable-dependency-tracking
      # When a transformation that inserts klee_range / klee_assume calls is
      # applied (e.g. input-prefix-pin.patch), the .o files reference those
      # symbols; -lklee-stubs (set in LDFLAGS by program-builder.nix) must
      # land after them in the link line. Move it from LDFLAGS to LIBS so
      # make's link rule places it correctly. Harmless when no transformation
      # is applied (no references → -Wl,--as-needed drops the stubs lib).
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      # SUBDIRS = lib po doc; doc/ would pull in texinfo via `make all`, so
      # build only what `make` (the binary) actually depends on: lib/'s
      # static archive (which carries BUILT_SOURCES like fnmatch.h, glob.h),
      # then the make binary itself.
      make -j$NIX_BUILD_CORES -C lib LIBS="$stubs_lib_flags"
      make -j$NIX_BUILD_CORES make LIBS="$stubs_lib_flags"
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "-n"
    "-f"
    "A"
    "--sym-files"
    "1"
    "40"
  ];
}

{pkgs, ...}: {
  name = "patch";
  version = "2.8";
  notes = "from paper learch";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/patch/patch-2.8.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/patch/patch-2.8.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-MIpJg/8yRSG5shMQv8I5jKhheY8CMHx565m7Dg0r+YA=";
  };

  build = {
    artifact = "src/patch";
    phase = ''
      # Normalize mtimes across the whole tree to one timestamp before
      # configure. eval-transformations stages the source with `cp -r`, which
      # rewrites mtimes and makes automake think the autotools-generated files
      # (aclocal.m4, configure, Makefile.in, config.hin) are stale relative to
      # their *.m4 / *.am sources. The maintainer rebuild rules then shell out
      # to aclocal/autoconf/automake/m4, which are absent at container runtime
      # (Error 127 in build-bitcode.sh). A single uniform mtime keeps no
      # prerequisite strictly newer than its generated target, so those rules
      # never fire; missing targets (.o, the binary) still build normally.
      # Harmless for the pristine baseline build (the tarball already ships
      # consistent mtimes) and it does not alter any source byte, so the
      # compiled bitcode is identical with or without this line.
      find . -exec touch -d @1 {} +
      # When a transformation that inserts klee_range / klee_assume calls is
      # applied (e.g. r1-input-prefix-pin.patch), the .o files reference those
      # symbols; -lklee-stubs (set in LDFLAGS by program-builder.nix) must land
      # AFTER them in the link line. patch's link rule is
      # `$(LINK) $(patch_OBJECTS) $(patch_LDADD) $(LIBS)`, so move the stubs
      # from LDFLAGS to LIBS to place them last. Harmless when no transformation
      # is applied (no references → -Wl,--as-needed drops the stubs lib).
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      # patch's configure doesn't recognize --disable-nls (no gettext gating);
      # leave it off. lib/ is the gnulib helper archive that src/patch links
      # against; build it first, then patch alone, skipping tests/.
      ./configure --disable-dependency-tracking
      make -j$NIX_BUILD_CORES -C lib LIBS="$stubs_lib_flags"
      make -j$NIX_BUILD_CORES -C src patch LIBS="$stubs_lib_flags"
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

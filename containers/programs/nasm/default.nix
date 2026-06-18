{pkgs, ...}: {
  name = "nasm";
  version = "3.01";
  notes = "from paper empc, cgs";

  src = pkgs.fetchurl {
    url = "https://github.com/netwide-assembler/nasm/archive/refs/tags/nasm-3.01.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/netwide-assembler/nasm/archive/refs/tags/nasm-3.01.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-ry8kHswGEgXXO6T3gfB10CXauuqwILZ2t9sUS/cBXW0=";
  };

  # GitHub source archive ships configure.ac, not configure — regenerate via
  # autogen.sh, which calls aclocal/autoheader/autoconf and (optionally) perl
  # for unconfig.pl.
  extraNativeBuildInputs = [pkgs.autoconf pkgs.automake pkgs.perl];

  build = {
    artifact = "nasm";
    phase = ''
      ./autogen.sh
      ./configure --without-zlib
      # If a transformation that inserts klee_range / klee_assume calls is
      # applied via eval-transformations, the .o files reference those
      # symbols. nasm's link rule is:
      #   $(CC) $(ALL_LDFLAGS) -o $@ $(NASM) $(MANIFEST) $(NASMLIB) $(LIBS)
      # so LDFLAGS (containing -lklee-stubs) is placed BEFORE the .o files.
      # That violates the left-to-right resolution order with -Wl,--as-needed.
      # Strip the stubs out of LDFLAGS (handled by program-builder.nix), then
      # add them to LIBS, which sits AFTER the .o files in the link line.
      # Harmless for the unmodified baseline (no references → stubs dropped).
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      # make passes LIBS through; it sits after the .o files in the link line.
      make -j$NIX_BUILD_CORES LIBS="$stubs_lib_flags"
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
    "--sym-files"
    "1"
    "100"
  ];
}

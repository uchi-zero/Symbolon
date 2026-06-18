{pkgs, ...}: {
  name = "bison";
  version = "3.8.2";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-BsnhO99+sk1M62tZIFpPZ8LH5yExGWREMP6C+9FKCrs=";
  };

  # bison build helpers shell out to m4 and perl; texinfo is needed because
  # the default `make` target builds doc/bison.info, and BUILT_SOURCES (the
  # gnulib-generated headers in lib/) are only produced as part of `all`.
  extraNativeBuildInputs = [pkgs.m4 pkgs.perl pkgs.texinfo];

  build = {
    artifact = "src/bison";
    phase = ''
      # eval-transformations copies the source tree before patching it, which
      # can make autotools inputs appear newer than their generated outputs.
      # Keep the shipped configure/Makefile.in files authoritative in the eval
      # container; transformations here patch generated C directly.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      find . \( -name configure -o -name aclocal.m4 -o -name config.h.in -o -name Makefile.in \) \
        -exec touch -h -d '2020-01-02 00:00:00' {} + 2>/dev/null || true

      # Bison's link rule places LDFLAGS before the object files, so the KLEE
      # stub library must be moved to LIBS to satisfy transformed objects that
      # reference klee_assume/klee_range.
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')

      ./configure --disable-nls --disable-dependency-tracking
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

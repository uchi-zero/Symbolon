{pkgs, ...}: {
  name = "combine";
  version = "0.4.0";
  notes = "";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/combine/combine-0.4.0.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/combine/combine-0.4.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-Vucc2cw4QE/E5trALV+3bolNSB+b6HGQcrH4sdivvhQ=";
  };

  build = {
    artifact = "src/combine";
    phase = ''
      # combine's configure aborts with "guile required but not found" when
      # the Guile binary isn't on PATH, even though the only consumer
      # (src/iguile.c) is gated on HAVE_LIBGUILE. Pre-seed the autoconf
      # cache to skip the path search; the subsequent -lguile link probe
      # fails on its own and leaves HAVE_LIBGUILE undefined, so iguile.c
      # compiles as a stub.
      export ac_cv_path_GUILE="$(command -v true)"
      ./configure --disable-nls --disable-dependency-tracking
      # Build the support lib first, then combine itself; skip doc/ (texinfo)
      # and tests/.
      make -j$NIX_BUILD_CORES -C lib
      make -j$NIX_BUILD_CORES -C src combine
    '';
  };

  # combine's 2002-era source has K&R prototypes and `fprintf(stderr,
  # program_name)`-style code that modern clang treats as -Werror by
  # default. Demote those classes back to warnings.
  bitcode.extraCFlags = [
    "-Wno-implicit-function-declaration"
    "-Wno-error=deprecated-non-prototype"
    "-Wno-error=format-security"
    "-Wno-error=pointer-sign"
    "-Wno-error=implicit-int"
  ];
  instrumented.extraCFlags = [
    "-Wno-implicit-function-declaration"
    "-Wno-error=deprecated-non-prototype"
    "-Wno-error=format-security"
    "-Wno-error=pointer-sign"
    "-Wno-error=implicit-int"
  ];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "2"
    "4"
    "A"
    "B"
    "--sym-files"
    "2"
    "50"
  ];
}

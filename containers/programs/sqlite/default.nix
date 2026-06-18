{pkgs, ...}: {
  name = "sqlite";
  version = "3.53.1";
  notes = "from paper learch, cgs";

  src = pkgs.fetchurl {
    url = "https://github.com/sqlite/sqlite/archive/refs/tags/version-3.53.1.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/sqlite/sqlite/archive/refs/tags/version-3.53.1.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-ceE+D6W5JbKKfFtVFQeejsIcc5sd3Jy7h+ZSRiYxfBU=";
  };

  build = {
    artifact = "sqlite3";
    phase = ''
      # --disable-tcl uses the in-tree jimsh0.c for code generation, so no
      # external tcl dep. Drop readline/load-extension to keep the link clean
      # under klee-uclibc; zlib auto-detect is a no-op without zlib in scope.
      ./configure \
        --disable-shared \
        --disable-tcl \
        --disable-readline \
        --disable-load-extension
      make -j$NIX_BUILD_CORES sqlite3
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-stdin"
    "20"
    "--sym-stdout"
  ];
}

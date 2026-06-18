{pkgs, ...}: {
  name = "transicc";
  version = "2.19";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://github.com/mm2/Little-CMS/releases/download/lcms2.19/lcms2-2.19.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/mm2/Little-CMS/releases/download/lcms2.19/lcms2-2.19.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-SefhNOQplzPdDtpDT6RomXooqz0z+jl8ZCsDZE9VIhY=";
  };

  build = {
    artifact = "utils/transicc/transicc";
    phase = ''
      # Static-only build skips the libtool wrapper script so the artifact at
      # utils/transicc/transicc is a real binary. Drop optional deps so the
      # other utils (tificc, jpgicc) don't try to link against libraries we
      # don't ship in the klee-uclibc env.
      ./configure \
        --disable-shared \
        --enable-static \
        --disable-dependency-tracking \
        --without-jpeg \
        --without-tiff \
        --without-zlib
      # Build only what transicc needs: the lcms2 static lib, then transicc.
      make -j$NIX_BUILD_CORES -C src
      make -j$NIX_BUILD_CORES -C utils/transicc
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

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
    "200"
    "--save-all-writes"
  ];
}

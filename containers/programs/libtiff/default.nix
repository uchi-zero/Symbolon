{pkgs, ...}: {
  name = "libtiff";
  version = "4.7.1";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "http://download.osgeo.org/libtiff/tiff-4.7.1.tar.gz";
    # Verified with: nix-prefetch-url http://download.osgeo.org/libtiff/tiff-4.7.1.tar.gz
    # then `nix hash to-sri --type sha256 <hex>` to get the SRI form below.
    hash = "sha256-9pjZTzED2oynQ42E4DRORT/gujt0huBMW/epo/q+m2k=";
  };

  build = {
    # Target tiffinfo — reads a TIFF and exercises the bulk of the parser
    # (directory walk, tag decoding, codec dispatch). The symbolic file
    # KLEE creates from --sym-files 1 300 is named "A" by convention,
    # which matches the literal "A" positional in symEnv below.
    artifact = "tools/tiffinfo";
    phase = ''
      # Disable every optional codec so the bitcode stays minimal and we
      # don't pull in JPEG/zlib/zstd/etc. paths KLEE can't model.
      ./configure \
        --disable-shared \
        --disable-cxx \
        --disable-docs \
        --disable-tests \
        --disable-jpeg \
        --disable-old-jpeg \
        --disable-jbig \
        --disable-lzma \
        --disable-zstd \
        --disable-libdeflate \
        --disable-pixarlog \
        --disable-webp \
        --disable-lerc \
        --without-x
      make -j$NIX_BUILD_CORES -C port
      make -j$NIX_BUILD_CORES -C libtiff
      make -j$NIX_BUILD_CORES -C tools tiffinfo
    '';
  };

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "3"
    "8"
    "A"
    "--sym-files"
    "1"
    "300"
  ];
}

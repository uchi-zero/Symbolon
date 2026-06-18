{pkgs, ...}: {
  name = "curl";
  version = "8.20.0";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://github.com/curl/curl/releases/download/curl-8_20_0/curl-8.20.0.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/curl/curl/releases/download/curl-8_20_0/curl-8.20.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-/FgZytP59Ugmaa3NxJp4LBXzbSoHFbOVsG2Rc1k9LcA=";
  };

  build = {
    artifact = "src/curl";
    phase = ''
      # Strip every optional dep so the binary links cleanly under
      # klee-uclibc — no TLS, no compression, no resolvers, no protocols
      # beyond the defaults the test inputs need.
      ./configure \
        --disable-shared \
        --enable-static \
        --disable-dependency-tracking \
        --disable-manual \
        --disable-docs \
        --disable-libcurl-option \
        --disable-threaded-resolver \
        --disable-ldap \
        --disable-ldaps \
        --without-ssl \
        --without-libpsl \
        --without-zlib \
        --without-zstd \
        --without-brotli \
        --without-libidn2 \
        --without-nghttp2 \
        --without-libssh2 \
        --without-libssh
      # Build only libcurl + the curl binary; skip docs/scripts SUBDIRS.
      make -j$NIX_BUILD_CORES -C lib
      make -j$NIX_BUILD_CORES -C src
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "4"
    "6"
    "--sym-args"
    "0"
    "1"
    "20"
  ];
}

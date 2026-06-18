{pkgs, ...}: {
  name = "tic";
  version = "6.6";
  notes = "from paper empc";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/ncurses/ncurses-6.6.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/ncurses/ncurses-6.6.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-NVtMu+2ICwOBoExGYXt2VuNiWF1S6c+Epn4gCbdJ/xE=";
  };

  build = {
    artifact = "progs/tic";
    phase = ''
      # Static-only build; drop every optional subdir we don't need so the
      # default `make` target doesn't pull in C++/Ada/manpages/tests.
      ./configure \
        --without-shared \
        --with-normal \
        --without-cxx \
        --without-cxx-binding \
        --without-ada \
        --without-tests \
        --without-manpages \
        --disable-db-install \
        --disable-pc-files
      # ncurses progs depend on the static lib + generated headers in
      # ncurses/ and include/. Build the library tree first, then tic alone.
      make -j$NIX_BUILD_CORES -C include
      make -j$NIX_BUILD_CORES -C ncurses
      make -j$NIX_BUILD_CORES -C progs tic
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  symEnv = [
    "--sym-args"
    "0"
    "2"
    "8"
    "A"
    "--sym-files"
    "1"
    "100"
  ];
}

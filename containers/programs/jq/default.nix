{pkgs, ...}: {
  name = "jq";
  version = "1.8.1";
  notes = "";

  src = pkgs.fetchurl {
    url = "https://github.com/jqlang/jq/archive/refs/tags/jq-1.8.1.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/jqlang/jq/archive/refs/tags/jq-1.8.1.tar.gz
    # then `nix hash to-sri --type sha256 <hex>` to get the SRI form below.
    hash = "sha256-UcTsCRMoHZF/Iqgj0d3PF8m8K+hF+smH1f4dOueN1qI=";
  };

  # GitHub archive ships configure.ac + Makefile.am only — no generated
  # configure script — so autoreconf has to run first. bison/flex feed the
  # AC_PROG_YACC + AC_CHECK_PROGS(LEX, flex lex) checks in configure.ac.
  extraNativeBuildInputs = [
    pkgs.autoconf
    pkgs.automake
    pkgs.libtool
    pkgs.bison
    pkgs.flex
  ];

  build = {
    # With --disable-shared, libtool skips the wrapper-script dance and the
    # final statically-linked ELF lands at ./jq directly.
    artifact = "jq";
    phase = ''
      # The GitHub tarball doesn't pull submodule content, so vendor/oniguruma
      # is empty — disable the regex backend rather than fetch the submodule.
      # vendor/decNumber is shipped in-tree so decnum support stays on.
      autoreconf -fi
      # --disable-shared so libjq is linked statically into the jq binary;
      # otherwise gllvm extracts only main.c's bitcode and every jq_* symbol
      # shows up as "undefined reference" under KLEE.
      ./configure \
        --without-oniguruma \
        --disable-maintainer-mode \
        --disable-docs \
        --disable-valgrind \
        --disable-shared \
        --enable-static
      # automake's BUILT_SOURCES (builtin.inc / config_opts.inc / version.h)
      # only fire on the default `all` target; building `jq` directly would
      # skip them, so generate them explicitly first.
      make -j$NIX_BUILD_CORES src/builtin.inc src/config_opts.inc src/version.h
      make -j$NIX_BUILD_CORES jq
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  # jq's CLI shape: `jq [options] <filter> [files...]`. The first argv is
  # symbolic up to 10 bytes — fits the bulk of common shapes: short flags
  # (`-r`, `-n`, `-c`), short long-flags up to `--indent`/`--stream`, and
  # chained filters like `.[]|.foo`, `select(.x)`, `keys|.[]`. JSON is fed
  # via 50 B of symbolic stdin so the JSON parser gets exercised too.
  symEnv = [
    "--sym-args"
    "1"
    "1"
    "10"
    "--sym-stdin"
    "50"
  ];
}

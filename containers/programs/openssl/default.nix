{pkgs, ...}: {
  name = "openssl";
  version = "4.0.0";
  notes = ''
    github source tarball (tag openssl-4.0.0). Heavy startup (provider +
    config init): the first terminating path / first ktest lands ~45-120s in,
    so a too-short budget yields 0 ktests with klee.exit=1 even though the
    bitcode is fine. Give it >=120s for a smoke run; a real sweep (1h+) is
    unaffected. Verified in-container: 120s nurs:covnew => 5425 lines, 1 ktest.
  '';

  src = pkgs.fetchurl {
    url = "https://github.com/openssl/openssl/archive/refs/tags/openssl-4.0.0.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/openssl/openssl/archive/refs/tags/openssl-4.0.0.tar.gz
    # then `nix hash to-sri --type sha256 <hex>` to get the SRI form below.
    hash = "sha256-5MQkhwF41jRsEjclVnQixxMYFvQdO3FnE3ALhM7eDu8=";
  };

  # OpenSSL's Configure and its generated-source build steps are Perl; the
  # eval sandbox has no perl on PATH otherwise.
  extraNativeBuildInputs = [pkgs.perl];

  build = {
    artifact = "apps/openssl";
    phase = ''
      # -lklee-stubs link order. OpenSSL's bin link rule is
      #   $(CC) $(BIN_LDFLAGS) -o apps/openssl <OBJECTS> -lssl -lcrypto $(BIN_EX_LIBS)
      # with the harness's $(LDFLAGS) (carrying -lklee-stubs) folded into
      # $(BIN_LDFLAGS), i.e. emitted BEFORE the objects. A source transformation
      # that inserts klee_range / klee_assume (e.g. an input-menu-pin in
      # apps/asn1parse.c) leaves those references in openssl-bin-asn1parse.o, so
      # the stub archive must come AFTER the objects or the gclang link fails
      # with "undefined reference to klee_range". OpenSSL's post-object slot is
      # $(EX_LIBS), fed by the LDLIBS env var (Configure: ex_libs <= env LDLIBS;
      # unix-Makefile.tmpl: EX_LIBS = $config{LDLIBS}); so pull -lklee-stubs (and
      # its -L) out of LDFLAGS and hand them to Configure as LDLIBS. Mirrors the
      # bc / sed / lua harnesses. No-op for the unpatched baseline: with nothing
      # referencing the klee intrinsics, the stub archive contributes no objects
      # in either slot, so the baseline program.bc is byte-identical.
      stubs=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+klee-intrinsic-stubs[^ ]*|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      export LDLIBS="$stubs"

      # KLEE executes LLVM bitcode, not x86 assembly, so the crypto must be
      # pure C: no-asm. no-shared statically links libcrypto.a + libssl.a into
      # apps/openssl, so all the bitcode embeds in the one binary get-bc reads
      # (a .so build would leave that bitcode outside the binary). no-threads
      # keeps it single-threaded under klee-uclibc; no-tests/no-docs skip work
      # we never link into the openssl CLI. Configure honours the env CC /
      # CFLAGS / LDFLAGS the harness exports (bitcode: -O0 -g + klee stubs;
      # cov: -fprofile-instr-generate), appending CFLAGS after the target
      # defaults so our -O0 wins over OpenSSL's default -O3.
      perl ./Configure linux-x86_64 \
        no-asm no-shared no-threads no-tests no-docs

      # build_generated must finish before the object compiles: many sources
      # include generated headers (opensslv.h, configuration.h, …) and a single
      # `make apps/openssl` races the header generation against the first
      # compile. Build the generated files first, then the CLI binary; the
      # apps/openssl target pulls in libcrypto.a + libssl.a as prerequisites.
      make -j$NIX_BUILD_CORES build_generated
      make -j$NIX_BUILD_CORES apps/openssl
    '';
  };

  # KLEE's module verifier rejects the linked bitcode when it is built with
  # -g: clang/gllvm emit a handful of synthesized `memcpy` calls with no !dbg
  # location, and LLVM's rule "inlinable call in a function with debug info
  # must have a !dbg location" then fails the whole module ("Broken module
  # found, compilation aborted"). The bitcode only ever runs inside KLEE, and
  # this harness measures coverage from the *separate* instrumented program.cov
  # binary (which keeps its own -g) — so dropping debug info from the bitcode
  # costs only KLEE's source-line error annotations, not coverage. -g0 is
  # appended after the builder's base "-O0 -g", so it wins and disables the
  # debug-info generation that trips the verifier.
  bitcode.extraCFlags = ["-g0"];

  withLibcxx = false;

  symEnv = [
    "asn1parse"
    "-inform"
    "DER"
    "--sym-stdin"
    "64"
  ];
}

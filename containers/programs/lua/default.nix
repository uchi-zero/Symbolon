{pkgs, ...}: {
  name = "lua";
  version = "5.4.7";
  notes = ''
    lua.org source tarball. The standalone interpreter runs piped (non-tty)
    stdin as a Lua chunk, so symbolic stdin drives the lexer -> parser ->
    bytecode VM directly. Built with the `generic` Makefile target (no
    LUA_USE_DLOPEN/-ldl, no readline, no -Wl,-E export-dynamic) to stay
    self-contained under klee-uclibc.
  '';

  src = pkgs.fetchurl {
    url = "https://www.lua.org/ftp/lua-5.4.7.tar.gz";
    # Verified with: nix-prefetch-url https://www.lua.org/ftp/lua-5.4.7.tar.gz
    # then `nix hash to-sri --type sha256 <hex>` to get the SRI form below.
    hash = "sha256-n79eKO+GxphY9tPTTszDLpEcGii0Eg/z6EqqcM+/HjA=";
  };

  build = {
    artifact = "src/lua";
    phase = ''
      # The `generic` target builds a portable POSIX lua with no dlopen and no
      # readline — both are wrong under symbolic execution (KLEE can't dlopen,
      # and readline pulls a terminal dependency).
      #
      # src/Makefile HARDCODES `CC= gcc -std=gnu99` with `=`, so it would
      # ignore the runner's CC (gclang for the bitcode pass, LLVM-16 clang for
      # cov) and build with gcc — leaving no embedded bitcode for get-bc to
      # extract. Pass CC="$CC -std=gnu99" on the make line to force the
      # harness compiler while keeping Lua's required C99 mode.
      #
      # Lua's CFLAGS is `-O2 ... $(SYSCFLAGS) $(MYCFLAGS)` and LDFLAGS is
      # `$(SYSLDFLAGS) $(MYLDFLAGS)`, both appended last — so the harness flags
      # in MYCFLAGS/MYLDFLAGS (bitcode: -O0 -g0 + klee include; cov:
      # -fprofile-instr-generate) win over Lua's -O2.
      #
      # The link rule (src/Makefile) is `$(CC) -o $@ $(LDFLAGS) $(LUA_O)
      # $(LUA_A) $(LIBS)` — it emits $(LDFLAGS) BEFORE the objects but $(LIBS)
      # AFTER them. A source transformation that calls klee_assume / klee_range
      # under KLEE_BITCODE (e.g. transformations/stdin-menu-pin.patch) references
      # the intrinsics from lauxlib.o, so -lklee-stubs must come AFTER the
      # objects or the gclang link fails with "undefined reference to
      # klee_assume". So move -lklee-stubs (and its -L) out of MYLDFLAGS and into
      # MYLIBS (LIBS= -lm $(SYSLIBS) $(MYLIBS), emitted last), exactly as the bc
      # entry does. The transformation-free baseline never references the
      # intrinsics, so this is harmless there (-Wl,--as-needed drops the unused
      # stub lib); the cov build carries no -lklee-stubs in LDFLAGS at all, so
      # the extraction is a no-op for it.
      stubs=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+klee-intrinsic-stubs[^ ]*|-lklee-stubs)' | tr '\n' ' '; } || true)
      ldflags_nostubs=$(echo "$LDFLAGS" | sed -E 's#-L[^ ]+klee-intrinsic-stubs[^ ]*##g; s#-lklee-stubs##g')
      make -C src generic \
        CC="$CC -std=gnu99" \
        MYCFLAGS="$CFLAGS" MYLDFLAGS="$ldflags_nostubs" MYLIBS="$stubs" \
        SYSCFLAGS="-DLUA_USE_POSIX" SYSLIBS=""
    '';
  };

  # Lua, like OpenSSL's bitcode, is built with -g and clang emits synthesized
  # memcpy calls without !dbg locations, which trips KLEE's module verifier
  # ("inlinable call in a function with debug info must have a !dbg location ->
  # Broken module"). Coverage comes from the separate instrumented program.cov
  # (which keeps its own -g), so -g0 on the bitcode costs only KLEE's
  # source-line error annotations. -g0 is appended after the builder's base
  # "-O0 -g", so it wins.
  bitcode.extraCFlags = ["-g0"];

  withLibcxx = false;

  # Symbolic stdin = the Lua script the interpreter reads and runs, so byte
  # count = how much syntax the lexer/parser/VM can reach. 64 bytes (2x the
  # initial smoke size) admits full statements and nested constructs, sized for
  # the long-budget searcher sweep (run-klee-searchers.sh: 12h, 32 GB KLEE
  # heap).
  #
  # WATCH THIS WIDTH. Lua's lexer reads stdin byte-by-byte and forks on the
  # character class at every position, so path count roughly squares as the
  # buffer grows. A short-budget gate confirms the asymmetry: --sym-stdin 32
  # emits a ktest by 45s, but --sym-stdin 64 produced 0 ktests / 0 completed
  # paths even at 180s (KLEE never advanced a state far enough to cover new
  # code). nurs:covnew at the full 12h budget may break through where 180s
  # could not — that is the bet here — but if the sweep's coverage trajectory
  # is still flat at 0 well into the run, drop back to 48 or the proven 32.
  symEnv = [
    "--sym-stdin"
    "64"
  ];

}

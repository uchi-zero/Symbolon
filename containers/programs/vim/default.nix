{pkgs, ...}: {
  name = "vim";
  version = "9.2.0458";
  notes = "";

  src = pkgs.fetchurl {
    url = "https://github.com/vim/vim/archive/refs/tags/v9.2.0458.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/vim/vim/archive/refs/tags/v9.2.0458.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri <hex>` to get the SRI form below.
    hash = "sha256-htU2VFQp86NCVsPwzHq91RMxagNwkNcVjt6fQW4DfrU=";
  };

  # vim links against libtinfo from ncurses for terminfo lookups even with
  # --with-features=tiny; the in-tree termlib fallback is not enough.
  extraBuildInputs = [pkgs.ncurses];

  build = {
    artifact = "src/vim";
    phase = ''
      # vim's link rule (src/Makefile) is
      #   $(CClink) $(ALL_LIB_DIRS) $(LDFLAGS) -o $(VIMTARGET) $(OBJ) $(ALL_LIBS)
      # i.e. $(LDFLAGS) is emitted BEFORE the objects, so the runner's
      # `-lklee-stubs` in LDFLAGS is searched too early and the klee_range /
      # klee_assume references that the menu-pin transformation puts in main.o
      # fail to resolve ("undefined reference to klee_range"). Move the stub lib
      # out of LDFLAGS and into the configure-time LIBS env var, which autoconf
      # threads into $(ALL_LIBS) — emitted AFTER $(OBJ) — so the final link is
      # `... $(OBJ) ... -lklee-stubs`. Harmless for the no-transformation build
      # (no klee_* refs) and for the cov build (LDFLAGS carries no stub lib
      # there). Mirrors the same fix in containers/programs/flex/default.nix.
      stubs_lib_flags=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')
      export LIBS="$stubs_lib_flags''${LIBS:+ $LIBS}"

      # Strip every optional surface so the bitcode is tractable: tiny feature
      # set, no GUI/X11/i18n, no script-language interpreters, no async
      # channels or :terminal emulation. Ex-mode editing still works.
      ./configure \
        --with-features=tiny \
        --disable-gui \
        --without-x \
        --disable-nls \
        --disable-channel \
        --disable-netbeans \
        --disable-terminal \
        --disable-canberra \
        --disable-libsodium \
        --disable-acl \
        --disable-selinux \
        --disable-smack \
        --disable-sysmouse
      make -j$NIX_BUILD_CORES -C src vim
    '';
  };

  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  # symEnv migration (see transformations/README.md "## symEnv migration"):
  # the previous harness `-E -s A --sym-files 1 40` made only the EDIT BUFFER
  # symbolic, so vim's command engine (normal.c / ex_cmds.c / regexp_* /
  # search.c / ops.c — ~66.5k of the ~89k reachable lines, 75% of the program)
  # was never driven and sat at ~2%: Ex commands arrive on stdin, which that
  # harness left empty/EOF. The plain baseline + best both plateaued near 9,231
  # lines (~10%), an in-scope ceiling proven across six rounds (r1-r6, archived
  # in the README) that could not be lifted without changing the symEnv.
  #
  # The new harness makes the COMMAND STREAM symbolic instead: `-N -E -s` is
  # nocompatible (enables vim-default paths + modelines) improved-Ex silent
  # batch mode reading Ex commands from stdin; `--sym-stdin 120` makes those 120
  # command bytes symbolic; no file arg means an empty start buffer that the
  # commands build and edit. KLEE has no concrete-file mechanism (only
  # --sym-files objects exist in its FS), so this no-file / symbolic-stdin shape
  # is the faithful realization of "fixed buffer, symbolic commands" — only the
  # command stream is symbolic, avoiding a second symbolic object. Oracle
  # evidence: a single concrete rich-command run already reaches ~13.6k lines
  # (vs the old 9,231 ceiling), and the engine lights up (ex_cmds.c 0->~500,
  # regexp_nfa.c 0->~1,100, ex_docmd.c ->~850). Like bash's --sym-stdin harness,
  # the raw symbolic command stream will path-explode in the Ex tokenizer; a
  # follow-up cmd-menu-pin transformation (next round) is expected, mirroring
  # bash's shipped cmd-menu-pin-v7.
  symEnv = [
    "-N"
    "-E"
    "-s"
    "--sym-stdin"
    "120"
  ];
}

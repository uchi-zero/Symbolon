{pkgs, ...}: {
  name = "cpio";
  version = "2.15";
  notes = "CLI archive tool: argv-fanout (-i/-o/-t) + per-format header parser";

  src = pkgs.fetchurl {
    url = "https://ftp.gnu.org/gnu/cpio/cpio-2.15.tar.gz";
    # Verified with: nix-prefetch-url https://ftp.gnu.org/gnu/cpio/cpio-2.15.tar.gz
    # then `nix hash to-sri --type sha256 <hex>` to get the SRI form below.
    hash = "sha256-76UO+YMTfu/AoC/bUVCdYkteMpXJgKoSfO7kGDRVSZ4=";
  };

  build = {
    artifact = "src/cpio";
    phase = ''
      # cpio 2.15 predates modern glibc's stat() macro/redirect. Its xstat
      # function pointer is declared with an empty (K&R) prototype `int
      # (*xstat) ();` and assigned `stat`/`lstat`; GCC/Clang on glibc >= 2.33
      # then reject the calls in copyout.c/copypass.c/util.c with "too many
      # arguments to function 'xstat'". Give the pointer its real prototype.
      # (The tarball stays pristine; this is a build-time source fix, not a
      # coverage transformation.)
      sed -i 's|^int (\*xstat) ();|int (*xstat) (const char *, struct stat *);|' src/global.c
      sed -i 's|^extern int (\*xstat) ();|extern int (*xstat) (const char *, struct stat *);|' src/extern.h

      # cpio is autotools with a pre-generated configure and no
      # AM_MAINTAINER_MODE. A later transformation that edits a *.c source can
      # make it newer than the generated autotools files and trip auto-remake
      # rules into running aclocal/automake/autoconf -- none of which exist in
      # the eval container. Normalize mtimes so the generated build files stay
      # "up to date" relative to the tree. No-op for the unpatched baseline.
      find . -exec touch -h -d '2020-01-01 00:00:00' {} + 2>/dev/null || true
      touch -d '2020-01-02 00:00:00' aclocal.m4 configure config.h.in \
            Makefile.in */Makefile.in 2>/dev/null || true

      # cpio's link rules (src/Makefile) emit $(LDFLAGS) before the objects but
      # $(LIBS) after them. A transformation that inserts klee_assume /
      # klee_make_symbolic leaves those references in the objects, so the stub
      # library must come AFTER them or the link fails with "undefined reference
      # to klee_assume". Pull -lklee-stubs out of LDFLAGS and pass it via LIBS.
      # Harmless for the baseline: with no references, -Wl,--as-needed drops it.
      stubs=$({ echo "$LDFLAGS" | grep -oE '(-L[^ ]+|-lklee-stubs)' | tr '\n' ' '; } || true)
      export LDFLAGS=$(echo "$LDFLAGS" | sed -E 's|-L[^ ]+klee-intrinsic-stubs[^ ]*||g; s|-lklee-stubs||g')

      # --with-rmt=FILE bakes FILE as the default rmt program AND suppresses
      # building the bundled rmt copy (cpio rejects --without-rmt: --with-rmt
      # demands an absolute path). The path is only the compiled-in default for
      # remote-tape access, which our symbolic file/stdin runs never take, so a
      # nominal /bin/rmt is fine. --disable-nls drops libintl/iconv so the
      # bitcode stays self-contained; cpio bundles gnulib for the rest. gnulib
      # (lib/) is a build-time dep of src/cpio, so build it first.
      ./configure --with-rmt=/bin/rmt --disable-nls
      # Build from the top level (default `all`): the recursive build generates
      # lib/configmake.h (which src/main.c includes) and sequences gnulib before
      # src/cpio. A `-C src` build alone misses that generated header. NLS off
      # and rmt excluded keep `all` from pulling texinfo/extra deps.
      make -j$NIX_BUILD_CORES LIBS="$stubs"
    '';
  };

  # gnulib emits a number of K&R-style and replacement-function decls that the
  # bitcode/instrumented clang treats as errors without this.
  bitcode.extraCFlags = ["-Wno-implicit-function-declaration"];
  instrumented.extraCFlags = ["-Wno-implicit-function-declaration"];

  withLibcxx = false;

  # Multi-mode archive CLI. Symbolic argv selects the operation mode (-i extract
  # / -t list + short flag combos); the 100-byte symbolic STDIN is the archive
  # that drives the per-format header parser (process_copy_in -> read_in_header
  # -> read_in_{old_ascii,new_ascii,binary,tar_header}). cpio in -i/-t mode reads
  # the archive from stdin (not from a file argument), so the symbolic bytes must
  # arrive on stdin to reach the parser -- a smoke run with --sym-files left
  # copyin.c at 0/1153 (only main.c argv parsing was exercised). 100 bytes covers
  # the odc header (76) + a short name and reaches into the 110-byte newc field
  # block.
  symEnv = [
    "--sym-args"
    "0"
    "2"
    "4"
    "--sym-stdin"
    "100"
  ];
}

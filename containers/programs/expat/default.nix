{pkgs, ...}: {
  name = "expat";
  version = "2.8.0";
  notes = "from paper cgs";

  src = pkgs.fetchurl {
    url = "https://github.com/libexpat/libexpat/releases/download/R_2_8_0/expat-2.8.0.tar.gz";
    # Verified with: nix-prefetch-url https://github.com/libexpat/libexpat/releases/download/R_2_8_0/expat-2.8.0.tar.gz
    # then `nix hash convert --hash-algo sha256 --to sri --from nix32 <hash>`.
    hash = "sha256-x87F9g6jpC53gHgcZ0UlXBmqPb/urlhka3Ey+I3CR4A=";
  };

  build = {
    artifact = "expat_parse";
    # libexpat has no standalone CLI we want to wire here (xmlwf would drag
    # in file-mapping and codepage code). We synthesize a minimal driver
    # that reads stdin and feeds it to XML_Parse in one shot. The symEnv
    # below makes stdin a 100-byte symbolic buffer.
    phase = ''
      ./configure \
        --without-xmlwf \
        --without-examples \
        --without-tests \
        --without-docbook \
        --disable-shared \
        --enable-static \
        --disable-dependency-tracking
      make -j$NIX_BUILD_CORES -C lib

      cat > parse_stdin.c <<'EOF'
      #include <stdio.h>
      #include "expat.h"

      int main(void) {
        XML_Parser p = XML_ParserCreate(NULL);
        if (!p) return 1;
        char buf[4096];
        size_t n = fread(buf, 1, sizeof(buf), stdin);
        enum XML_Status s = XML_Parse(p, buf, (int)n, /*isFinal=*/1);
        XML_ParserFree(p);
        return s == XML_STATUS_OK ? 0 : 2;
      }
      EOF
      $CC $CFLAGS -Ilib parse_stdin.c lib/.libs/libexpat.a -o expat_parse $LDFLAGS
    '';
  };

  withLibcxx = false;

  symEnv = ["--sym-stdin" "100"];

}

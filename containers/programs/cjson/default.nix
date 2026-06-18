{pkgs, ...}: {
  name = "cjson";
  version = "1.7.19";
  notes = "from paper learch";

  src = pkgs.fetchurl {
    url = "https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.19.tar.gz";
    # Verified with: nix-prefetch-url <url> then `nix hash to-sri --type sha256 <hex>`.
    hash = "sha256-f6YW4wRu36eiijLV+erP0j+SkA/h+MzZiMFmLzBFRWI=";
  };

  build = {
    artifact = "cjson_parse";
    # cJSON has no standalone CLI; we synthesize a minimal driver that reads
    # the file named by argv[1] and runs it through cJSON_Parse. The symEnv
    # below makes that file the symbolic 100-byte "A" KLEE creates.
    phase = ''
      cat > parse_file.c <<'EOF'
      #include <stdio.h>
      #include <stdlib.h>
      #include "cJSON.h"

      int main(int argc, char **argv) {
        if (argc < 2) return 1;
        FILE *f = fopen(argv[1], "rb");
        if (!f) return 2;
        if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 3; }
        long n = ftell(f);
        if (n < 0) { fclose(f); return 4; }
        rewind(f);
        char *buf = malloc((size_t)n + 1);
        if (!buf) { fclose(f); return 5; }
        size_t r = fread(buf, 1, (size_t)n, f);
        buf[r] = '\0';
        fclose(f);
        cJSON *j = cJSON_Parse(buf);
        int ok = (j != NULL);
        if (j) cJSON_Delete(j);
        free(buf);
        return ok ? 0 : 6;
      }
      EOF
      $CC $CFLAGS parse_file.c cJSON.c -o cjson_parse $LDFLAGS
    '';
  };

  withLibcxx = false;

  # A literal positional → KLEE's symbolic file is named "A" by convention.
  # --sym-files 1 100 : 1 symbolic input file of 100 bytes.
  symEnv = [
    "A"
    "--sym-files"
    "1"
    "100"
    "yes"
  ];
}

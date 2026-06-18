#include <string>
#include <symbolon/Paths.h>
#include <symbolon/TestCheck.h>

int main() {
  using symbolon::joinSourcePath;
  using symbolon::looksLikeSystemPath;

  SYMBOLON_CHECK(looksLikeSystemPath("/usr/include/stdio.h"));
  SYMBOLON_CHECK(looksLikeSystemPath(
      "/nix/store/abc123-glibc-2.40-66/include/byteswap.h"));
  SYMBOLON_CHECK(looksLikeSystemPath("/some/path/klee/runtime/POSIX/stub.c"));
  SYMBOLON_CHECK(
      looksLikeSystemPath("/home/x/klee-uclibc/libc/stdio/printf.c"));
  SYMBOLON_CHECK(
      looksLikeSystemPath("/build/llvm/projects/libcxx/src/string.cpp"));
  SYMBOLON_CHECK(
      looksLikeSystemPath("/build/llvm/projects/libcxxabi/src/abort.cpp"));

  SYMBOLON_CHECK(!looksLikeSystemPath(
      "/home/u/symbolon/containers/programs/hello/source/hello.c"));
  SYMBOLON_CHECK(!looksLikeSystemPath("hello.c"));
  SYMBOLON_CHECK(!looksLikeSystemPath(""));
  // /usr is a prefix-match, not a substring:
  SYMBOLON_CHECK(!looksLikeSystemPath("/home/usr/code.c"));

  // joinSourcePath: relative filename + directory composes them with a
  // single separator.
  SYMBOLON_CHECK(joinSourcePath("/home/u/proj", "foo.c") ==
                 "/home/u/proj/foo.c");
  SYMBOLON_CHECK(joinSourcePath("/home/u/proj/", "foo.c") ==
                 "/home/u/proj/foo.c");
  // Absolute filename: directory is ignored.
  SYMBOLON_CHECK(joinSourcePath("/anything", "/usr/include/stdio.h") ==
                 "/usr/include/stdio.h");
  // Empty directory: filename returned verbatim.
  SYMBOLON_CHECK(joinSourcePath("", "foo.c") == "foo.c");
  // Empty filename: empty result (caller treats as "no debug info").
  SYMBOLON_CHECK(joinSourcePath("/anywhere", "").empty());
  SYMBOLON_CHECK(joinSourcePath("", "").empty());

  return 0;
}

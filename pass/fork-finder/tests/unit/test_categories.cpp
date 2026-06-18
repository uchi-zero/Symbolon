#include "ForkFinder.h"
#include <iostream>
#include <symbolon/TestCheck.h>

using namespace forkfinder;

static void test_known_allowed() {
  // Representative sample spanning every semantic group in
  // kLibraryLoopFunctions; not exhaustive on purpose, to avoid drift with
  // the canonical list in Categories.cpp.
  for (const char *name : {
           "memcmp",     // mem-byte loop
           "memmem",     // mem-byte loop, less obvious
           "strcmp",     // string compare
           "strcasecmp", // string compare (case-insensitive)
           "strlen",     // string measure
           "strcpy",     // string copy
           "strncat",    // string cat
           "strchr",     // string search
           "strstr",     // string search (multi-char)
           "strtok_r",   // string search with underscore
           "memcpy",     // mem copy
           "memset",     // mem set
           "bzero",      // legacy mem set
       }) {
    SYMBOLON_CHECK(isLibraryLoopFunction(name));
  }
}

static void test_non_members() {
  for (const char *name : {
           "malloc",
           "calloc",
           "free",
           "realloc",
           "klee_make_symbolic",
           "klee_int",
           "klee_range",
           "printf",
           "puts",
           "exit",
           "abort",
           "",
           "MEMCMP",
           " memcmp",
           "memcmp ",
       }) {
    SYMBOLON_CHECK(!isLibraryLoopFunction(name));
  }
}

int main() {
  test_known_allowed();
  test_non_members();
  std::cout << "test_categories: ok\n";
  return 0;
}

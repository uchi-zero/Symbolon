#include "ForkFinder.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/StringSet.h>

#include <array>

namespace forkfinder {
namespace {

constexpr std::array<llvm::StringLiteral, 29> kLibraryLoopFunctions{
    // mem-byte loops
    "memcmp",
    "bcmp",
    "memchr",
    "memrchr",
    "memmem",
    // string compare / measure
    "strcmp",
    "strncmp",
    "strcasecmp",
    "strncasecmp",
    "strlen",
    "strnlen",
    // string copy / cat
    "strcpy",
    "strncpy",
    "strcat",
    "strncat",
    // string search
    "strchr",
    "strrchr",
    "strstr",
    "strcasestr",
    "strspn",
    "strcspn",
    "strpbrk",
    "strtok",
    "strtok_r",
    // mem copy / set
    "memcpy",
    "memmove",
    "memset",
    "bzero",
    "bcopy",
};

} // namespace

bool isLibraryLoopFunction(llvm::StringRef name) {
  static const llvm::StringSet<> kSet = [] {
    llvm::StringSet<> s;
    for (auto entry : kLibraryLoopFunctions)
      s.insert(entry);
    return s;
  }();
  return kSet.contains(name);
}

} // namespace forkfinder

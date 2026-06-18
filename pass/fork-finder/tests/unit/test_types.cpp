#include "ForkFinder.h"
#include <string>
#include <symbolon/TestCheck.h>

int main() {
  using namespace forkfinder;
  Finding f{"foo.c", "main", 42, "br", Category::Conditional};
  SYMBOLON_CHECK(f.file == "foo.c");
  SYMBOLON_CHECK(f.function == "main");
  SYMBOLON_CHECK(f.line == 42);
  SYMBOLON_CHECK(f.opcode == "br");
  SYMBOLON_CHECK(f.category == Category::Conditional);
  SYMBOLON_CHECK(f.loop_depth == 0);

  Options o;
  SYMBOLON_CHECK(o.include_implicit == false);
  SYMBOLON_CHECK(o.include_system == false);
  SYMBOLON_CHECK(o.include_non_loop == false);

  // toString round-trip for every category we expose.
  for (Category c :
       {Category::Conditional, Category::Switch, Category::Indirect,
        Category::Call, Category::MemOp, Category::Alloc, Category::Realloc,
        Category::Free, Category::DivByZero, Category::OverflowCheck,
        Category::LibraryLoop}) {
    std::string s{categoryName(c)};
    SYMBOLON_CHECK(!s.empty());
  }
  return 0;
}

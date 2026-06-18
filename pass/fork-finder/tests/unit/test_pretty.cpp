#include "ForkFinder.h"
#include <iostream>
#include <string>
#include <symbolon/TestCheck.h>

int main() {
  using namespace forkfinder;
  std::vector<Finding> fs{
      {"a.c", "f", 1, "br", Category::Conditional},
  };
  auto j = renderReport(fs, Options{}, "a.bc");
  std::string flat = j.dump();
  std::string pretty = j.dump(2);
  SYMBOLON_CHECK(flat.find('\n') == std::string::npos);
  SYMBOLON_CHECK(pretty.find('\n') != std::string::npos);
  SYMBOLON_CHECK(pretty.find("  \"") != std::string::npos);
  std::cout << "test_pretty: ok\n";
  return 0;
}

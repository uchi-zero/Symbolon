#include "StateKillerFinder.h"
#include <iostream>
#include <string>
#include <symbolon/TestCheck.h>
#include <vector>

int main() {
  using namespace statekillerfinder;
  std::vector<Finding> fs{
      {"a.c", "f", 1, "call", "printf", Category::ExternCallSymArg},
  };
  Options o;
  o.enabled = {Category::ExternCallSymArg};
  auto j = renderReport(fs, o, "a.bc");
  std::string flat = j.dump();
  std::string pretty = j.dump(2);
  SYMBOLON_CHECK(flat.find('\n') == std::string::npos);
  SYMBOLON_CHECK(pretty.find('\n') != std::string::npos);
  SYMBOLON_CHECK(pretty.find("  \"") != std::string::npos);
  std::cout << "test_pretty: ok\n";
  return 0;
}

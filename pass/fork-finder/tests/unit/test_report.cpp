#include "ForkFinder.h"
#include <iostream>
#include <symbolon/TestCheck.h>
#include <vector>

using namespace forkfinder;
using json = nlohmann::ordered_json;

static void test_dedup_same_line_two_opcodes() {
  std::vector<Finding> fs{
      {"src/x.c", "main", 47, "load", Category::MemOp, 0},
      {"src/x.c", "main", 47, "udiv", Category::DivByZero, 0},
  };
  Options o;
  o.include_implicit = true;
  json r = renderReport(fs, o, "x.bc");

  auto &line = r["files"]["src/x.c"]["functions"]["main"]["lines"]["47"];
  SYMBOLON_CHECK(line["categories"] == json::array({"DivByZero", "MemOp"}));
  SYMBOLON_CHECK(line["opcodes"] == json::array({"load", "udiv"}));
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_category"]["MemOp"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_category"]["DivByZero"] == 1);
}

static void test_dedup_same_category_two_opcodes() {
  // Two MemOp findings on the same line (load + store) collapse to one
  // line entry; by_category.MemOp counts the line entry, not raw findings.
  std::vector<Finding> fs{
      {"src/x.c", "main", 10, "load", Category::MemOp, 0},
      {"src/x.c", "main", 10, "store", Category::MemOp, 0},
  };
  json r = renderReport(fs, Options{}, "x.bc");
  auto &line = r["files"]["src/x.c"]["functions"]["main"]["lines"]["10"];
  SYMBOLON_CHECK(line["categories"] == json::array({"MemOp"}));
  SYMBOLON_CHECK(line["opcodes"] == json::array({"load", "store"}));
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_category"]["MemOp"] == 1);
}

static void test_implicit_excluded_by_default() {
  std::vector<Finding> fs{
      {"a.c", "f", 1, "udiv", Category::DivByZero, 0},
  };
  Options o; // include_implicit == false
  json r = renderReport(fs, o, "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 0);
  SYMBOLON_CHECK(!r["summary"]["by_category"].contains("DivByZero"));
  SYMBOLON_CHECK(r["files"].empty());
}

static void test_unlocated_bucketing() {
  std::vector<Finding> fs{
      {"", "f", 0, "br", Category::Conditional, 0},
      {"a.c", "f", 10, "br", Category::Conditional, 0},
  };
  json r = renderReport(fs, Options{}, "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["unlocated_by_category"]["Conditional"] == 1);
  SYMBOLON_CHECK(r["files"].contains("a.c"));
}

static void test_line_keys_numeric_order() {
  std::vector<Finding> fs{
      {"a.c", "f", 100, "br", Category::Conditional, 0},
      {"a.c", "f", 9, "br", Category::Conditional, 0},
      {"a.c", "f", 12, "br", Category::Conditional, 0},
  };
  json r = renderReport(fs, Options{}, "a.bc");
  std::string s = r.dump();
  auto p9 = s.find("\"9\"");
  auto p12 = s.find("\"12\"");
  auto p100 = s.find("\"100\"");
  SYMBOLON_CHECK(p9 != std::string::npos && p12 != std::string::npos &&
                 p100 != std::string::npos);
  SYMBOLON_CHECK(p9 < p12 && p12 < p100);
}

static void test_summary_lists_all_default_categories() {
  std::vector<Finding> fs{};
  json r = renderReport(fs, Options{}, "empty.bc");
  for (auto name : {"Conditional", "Switch", "Indirect", "Call", "MemOp",
                    "Alloc", "Realloc", "Free", "LibraryLoop"}) {
    SYMBOLON_CHECK(r["summary"]["by_category"].contains(name));
    SYMBOLON_CHECK(r["summary"]["by_category"][name] == 0);
  }
  SYMBOLON_CHECK(!r["summary"]["by_category"].contains("DivByZero"));
  SYMBOLON_CHECK(!r["summary"]["by_category"].contains("OverflowCheck"));
}

static void test_system_skip_default() {
  std::vector<Finding> fs{
      {"/usr/include/stdio.h", "printf", 100, "br", Category::Conditional, 0},
      {"src/main.c", "main", 10, "br", Category::Conditional, 0},
  };
  Options o; // include_system == false
  json r = renderReport(fs, o, "main.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["system_skipped_by_category"]["Conditional"] ==
                 1);
  SYMBOLON_CHECK(!r["files"].contains("/usr/include/stdio.h"));
}

static void test_system_skip_disabled_by_flag() {
  std::vector<Finding> fs{
      {"/usr/include/stdio.h", "printf", 100, "br", Category::Conditional, 0},
  };
  Options o;
  o.include_system = true;
  json r = renderReport(fs, o, "main.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(!r["summary"].contains("system_skipped_by_category"));
}

static void test_schema_version_is_2() {
  std::vector<Finding> fs{};
  json r = renderReport(fs, Options{}, "empty.bc");
  SYMBOLON_CHECK(r["schema_version"] == 2);
}

static void test_line_carries_loop_depth() {
  std::vector<Finding> fs{
      {"a.c", "f", 5, "br", Category::Conditional, 2},
  };
  json r = renderReport(fs, Options{}, "a.bc");
  auto &line = r["files"]["a.c"]["functions"]["f"]["lines"]["5"];
  SYMBOLON_CHECK(line["loop_depth"] == 2);
}

static void test_loop_depth_aggregates_to_max() {
  std::vector<Finding> fs{
      {"a.c", "f", 7, "load", Category::MemOp, 1},
      {"a.c", "f", 7, "store", Category::MemOp, 3},
      {"a.c", "f", 7, "br", Category::Conditional, 2},
  };
  json r = renderReport(fs, Options{}, "a.bc");
  auto &line = r["files"]["a.c"]["functions"]["f"]["lines"]["7"];
  SYMBOLON_CHECK(line["loop_depth"] == 3);
}

static void test_library_loop_kept_at_depth_zero_under_default() {
  std::vector<Finding> fs{
      {"a.c", "f", 3, "call", Category::LibraryLoop, 0},
  };
  json r = renderReport(fs, Options{}, "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_category"]["LibraryLoop"] == 1);
  auto &line = r["files"]["a.c"]["functions"]["f"]["lines"]["3"];
  SYMBOLON_CHECK(line["categories"] == json::array({"LibraryLoop"}));
  SYMBOLON_CHECK(line["loop_depth"] == 0);
}

int main() {
  test_dedup_same_line_two_opcodes();
  test_dedup_same_category_two_opcodes();
  test_implicit_excluded_by_default();
  test_unlocated_bucketing();
  test_line_keys_numeric_order();
  test_summary_lists_all_default_categories();
  test_system_skip_default();
  test_system_skip_disabled_by_flag();
  test_schema_version_is_2();
  test_line_carries_loop_depth();
  test_loop_depth_aggregates_to_max();
  test_library_loop_kept_at_depth_zero_under_default();
  std::cout << "test_report: all passed\n";
  return 0;
}

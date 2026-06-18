#include "StateKillerFinder.h"
#include <iostream>
#include <string>
#include <symbolon/TestCheck.h>
#include <vector>

using namespace statekillerfinder;
using json = nlohmann::ordered_json;

static Options defaultOpts() {
  Options o;
  o.enabled = {Category::ExternCallSymArg};
  return o;
}

static void test_basic_aggregation() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_category"]["ExternCallSymArg"] == 1);
  auto &line = r["files"]["a.c"]["functions"]["main"]["lines"]["7"];
  SYMBOLON_CHECK(line["categories"] == json::array({"ExternCallSymArg"}));
  SYMBOLON_CHECK(line["callees"] == json::array({"printf"}));
}

static void test_dedup_two_callees_same_line() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
      {"a.c", "main", 7, "call", "fprintf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_category"]["ExternCallSymArg"] == 1);
  auto &line = r["files"]["a.c"]["functions"]["main"]["lines"]["7"];
  SYMBOLON_CHECK(line["callees"] == json::array({"fprintf", "printf"}));
}

static void test_pointer_category_gated_by_flag() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "puts", Category::ExternCallSymPtr},
  };
  // Default options: ExternCallSymPtr not in enabled set.
  json r1 = renderReport(fs, defaultOpts(), "a.bc");
  SYMBOLON_CHECK(r1["summary"]["total_findings"] == 0);
  SYMBOLON_CHECK(r1["files"].empty());

  Options o = defaultOpts();
  o.enabled.insert(Category::ExternCallSymPtr);
  json r2 = renderReport(fs, o, "a.bc");
  SYMBOLON_CHECK(r2["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r2["summary"]["by_category"]["ExternCallSymPtr"] == 1);
}

static void test_indirect_category_gated_by_flag() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "<indirect>", Category::ExternCallIndirect},
  };
  json r1 = renderReport(fs, defaultOpts(), "a.bc");
  SYMBOLON_CHECK(r1["summary"]["total_findings"] == 0);

  Options o = defaultOpts();
  o.enabled.insert(Category::ExternCallIndirect);
  json r2 = renderReport(fs, o, "a.bc");
  SYMBOLON_CHECK(r2["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r2["summary"]["by_category"]["ExternCallIndirect"] == 1);
}

static void test_unlocated_bucketing() {
  std::vector<Finding> fs{
      {"", "main", 0, "call", "printf", Category::ExternCallSymArg},
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(r["summary"]["unlocated_by_category"]["ExternCallSymArg"] ==
                 1);
}

static void test_unlocated_by_severity_bucketing() {
  std::vector<Finding> fs{
      {"", "main", 0, "call", "printf", Category::ExternCallSymArg}, // likely
      {"", "main", 0, "call", "abort", Category::ProgramTerminator}, // definite
      {"", "main", 0, "call", "p", Category::ExternCallIndirect}, // conditional
  };
  Options o;
  o.enabled = {Category::ExternCallSymArg, Category::ProgramTerminator,
               Category::ExternCallIndirect};
  json r = renderReport(fs, o, "a.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 0);
  auto &bs = r["summary"]["unlocated_by_severity"];
  SYMBOLON_CHECK(bs["definite"] == 1);
  SYMBOLON_CHECK(bs["likely"] == 1);
  SYMBOLON_CHECK(bs["conditional"] == 1);
}

static void test_unlocated_by_severity_omitted_when_empty() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "a.bc");
  SYMBOLON_CHECK(!r["summary"].contains("unlocated_by_severity"));
  SYMBOLON_CHECK(!r["summary"].contains("unlocated_by_category"));
}

static void test_system_skip_default() {
  std::vector<Finding> fs{
      {"/usr/include/stdio.h", "main", 7, "call", "printf",
       Category::ExternCallSymArg},
      {"src/main.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "main.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(
      r["summary"]["system_skipped_by_category"]["ExternCallSymArg"] == 1);
  SYMBOLON_CHECK(!r["files"].contains("/usr/include/stdio.h"));
}

static void test_system_skipped_by_severity_bucketing() {
  std::vector<Finding> fs{
      {"/usr/include/stdio.h", "main", 7, "call", "printf",
       Category::ExternCallSymArg}, // likely
      {"/nix/store/abc/include/x.h", "main", 8, "call", "abort",
       Category::ProgramTerminator}, // definite
  };
  Options o;
  o.enabled = {Category::ExternCallSymArg, Category::ProgramTerminator};
  json r = renderReport(fs, o, "main.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 0);
  auto &bs = r["summary"]["system_skipped_by_severity"];
  SYMBOLON_CHECK(bs["likely"] == 1);
  SYMBOLON_CHECK(bs["definite"] == 1);
  SYMBOLON_CHECK(!bs.contains("conditional"));
}

static void test_system_skipped_by_severity_omitted_when_empty() {
  std::vector<Finding> fs{
      {"src/main.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "main.bc");
  SYMBOLON_CHECK(!r["summary"].contains("system_skipped_by_severity"));
  SYMBOLON_CHECK(!r["summary"].contains("system_skipped_by_category"));
}

static void test_system_skip_disabled_by_flag() {
  std::vector<Finding> fs{
      {"/nix/store/abc/include/stdio.h", "main", 7, "call", "printf",
       Category::ExternCallSymArg},
  };
  Options o = defaultOpts();
  o.include_system = true;
  json r = renderReport(fs, o, "main.bc");
  SYMBOLON_CHECK(r["summary"]["total_findings"] == 1);
  SYMBOLON_CHECK(!r["summary"].contains("system_skipped_by_category"));
  SYMBOLON_CHECK(!r["summary"].contains("system_skipped_by_severity"));
}

static void test_line_keys_numeric_order() {
  std::vector<Finding> fs{
      {"a.c", "f", 100, "call", "printf", Category::ExternCallSymArg},
      {"a.c", "f", 9, "call", "printf", Category::ExternCallSymArg},
      {"a.c", "f", 12, "call", "printf", Category::ExternCallSymArg},
  };
  json r = renderReport(fs, defaultOpts(), "a.bc");
  std::string s = r.dump();
  auto p9 = s.find("\"9\"");
  auto p12 = s.find("\"12\"");
  auto p100 = s.find("\"100\"");
  SYMBOLON_CHECK(p9 != std::string::npos && p12 != std::string::npos &&
                 p100 != std::string::npos);
  SYMBOLON_CHECK(p9 < p12 && p12 < p100);
}

static void test_severities_emitted_per_line() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  Options o;
  o.enabled = {Category::ExternCallSymArg};
  json r = renderReport(fs, o, "a.bc");
  auto &line = r["files"]["a.c"]["functions"]["main"]["lines"]["7"];
  SYMBOLON_CHECK(line["severities"] == json::array({"likely"}));
}

static void test_by_severity_summary() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
      {"a.c", "main", 8, "call", "abort", Category::ProgramTerminator},
  };
  Options o;
  o.enabled = {Category::ExternCallSymArg, Category::ProgramTerminator};
  json r = renderReport(fs, o, "a.bc");
  SYMBOLON_CHECK(r["summary"]["by_severity"]["likely"] == 1);
  SYMBOLON_CHECK(r["summary"]["by_severity"]["definite"] == 1);
}

static void test_mixed_severity_same_line() {
  std::vector<Finding> fs{
      {"a.c", "main", 7, "call", "abort", Category::ProgramTerminator},
      {"a.c", "main", 7, "call", "printf", Category::ExternCallSymArg},
  };
  Options o;
  o.enabled = {Category::ExternCallSymArg, Category::ProgramTerminator};
  json r = renderReport(fs, o, "a.bc");
  auto &line = r["files"]["a.c"]["functions"]["main"]["lines"]["7"];
  SYMBOLON_CHECK(line["severities"] == json::array({"definite", "likely"}));
}

static void test_schema_version_is_two() {
  std::vector<Finding> fs;
  Options o;
  o.enabled = {Category::ExternCallSymArg};
  json r = renderReport(fs, o, "a.bc");
  SYMBOLON_CHECK(r["schema_version"] == 2);
}

int main() {
  test_basic_aggregation();
  test_dedup_two_callees_same_line();
  test_pointer_category_gated_by_flag();
  test_indirect_category_gated_by_flag();
  test_unlocated_bucketing();
  test_unlocated_by_severity_bucketing();
  test_unlocated_by_severity_omitted_when_empty();
  test_system_skip_default();
  test_system_skipped_by_severity_bucketing();
  test_system_skipped_by_severity_omitted_when_empty();
  test_system_skip_disabled_by_flag();
  test_line_keys_numeric_order();
  test_severities_emitted_per_line();
  test_by_severity_summary();
  test_mixed_severity_same_line();
  test_schema_version_is_two();
  std::cout << "test_report: all passed\n";
  return 0;
}

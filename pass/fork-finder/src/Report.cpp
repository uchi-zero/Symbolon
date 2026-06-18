#include "ForkFinder.h"

#include <symbolon/Paths.h>

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace forkfinder {

std::string_view categoryName(Category c) {
  switch (c) {
  case Category::Conditional:
    return "Conditional";
  case Category::Switch:
    return "Switch";
  case Category::Indirect:
    return "Indirect";
  case Category::Call:
    return "Call";
  case Category::MemOp:
    return "MemOp";
  case Category::Alloc:
    return "Alloc";
  case Category::Realloc:
    return "Realloc";
  case Category::Free:
    return "Free";
  case Category::DivByZero:
    return "DivByZero";
  case Category::OverflowCheck:
    return "OverflowCheck";
  case Category::LibraryLoop:
    return "LibraryLoop";
  }
  return "Unknown";
}

namespace {

bool isImplicit(Category c) {
  return c == Category::DivByZero || c == Category::OverflowCheck;
}

bool categoryEnabled(Category c, const Options &opts) {
  if (isImplicit(c))
    return opts.include_implicit;
  return true;
}

std::vector<Category> defaultSummaryCategories(const Options &opts) {
  std::vector<Category> out{
      Category::Conditional, Category::Switch, Category::Indirect,
      Category::Call,        Category::MemOp,  Category::Alloc,
      Category::Realloc,     Category::Free,   Category::LibraryLoop,
  };
  if (opts.include_implicit) {
    out.push_back(Category::DivByZero);
    out.push_back(Category::OverflowCheck);
  }
  return out;
}

struct LineAgg {
  std::set<std::string> categories;
  std::set<std::string> opcodes;
  unsigned max_loop_depth = 0;
};

} // namespace

nlohmann::ordered_json renderReport(const std::vector<Finding> &findings,
                                    const Options &opts,
                                    std::string_view moduleName) {
  using json = nlohmann::ordered_json;

  // file -> function -> line -> aggregate
  std::map<std::string, std::map<std::string, std::map<unsigned, LineAgg>>>
      tree;

  std::map<std::string, std::size_t> by_category;
  std::map<std::string, std::size_t> unlocated_by_category;
  std::map<std::string, std::size_t> system_skipped_by_category;
  std::size_t total_located = 0;

  // Pre-populate by_category for stable diff.
  for (Category c : defaultSummaryCategories(opts)) {
    by_category[std::string(categoryName(c))] = 0;
  }

  for (const Finding &f : findings) {
    if (!categoryEnabled(f.category, opts))
      continue;
    std::string cname(categoryName(f.category));
    if (f.file.empty()) {
      unlocated_by_category[cname] += 1;
      continue;
    }
    if (!opts.include_system && symbolon::looksLikeSystemPath(f.file)) {
      system_skipped_by_category[cname] += 1;
      continue;
    }
    auto &agg = tree[f.file][f.function][f.line];
    bool line_was_empty = agg.categories.empty() && agg.opcodes.empty();
    bool new_category = agg.categories.insert(cname).second;
    agg.opcodes.insert(f.opcode);
    if (f.loop_depth > agg.max_loop_depth)
      agg.max_loop_depth = f.loop_depth;
    if (new_category) {
      // Count once per (file, function, line, category) so the summary
      // matches what is actually visible in the files tree.
      by_category[cname] += 1;
    }
    if (line_was_empty) {
      // Count once per (file, function, line) so total_findings equals
      // the number of line entries under "files".
      total_located += 1;
    }
  }

  json out = json::object();
  out["schema_version"] = 2;
  out["module"] = std::string(moduleName);

  json summary = json::object();
  summary["total_findings"] = total_located;
  json bc = json::object();
  for (auto &kv : by_category)
    bc[kv.first] = kv.second;
  summary["by_category"] = bc;

  if (!unlocated_by_category.empty()) {
    json uc = json::object();
    for (auto &kv : unlocated_by_category)
      uc[kv.first] = kv.second;
    summary["unlocated_by_category"] = uc;
  }

  if (!system_skipped_by_category.empty()) {
    json sc = json::object();
    for (auto &kv : system_skipped_by_category)
      sc[kv.first] = kv.second;
    summary["system_skipped_by_category"] = sc;
  }

  out["summary"] = summary;

  json files = json::object();
  for (auto &fkv : tree) {
    json fns = json::object();
    for (auto &fnkv : fkv.second) {
      json lines = json::object();
      for (auto &lkv : fnkv.second) {
        json entry = json::object();
        entry["categories"] = json::array();
        for (auto &c : lkv.second.categories)
          entry["categories"].push_back(c);
        entry["opcodes"] = json::array();
        for (auto &o : lkv.second.opcodes)
          entry["opcodes"].push_back(o);
        entry["loop_depth"] = lkv.second.max_loop_depth;
        lines[std::to_string(lkv.first)] = entry;
      }
      fns[fnkv.first] = json::object({{"lines", lines}});
    }
    files[fkv.first] = json::object({{"functions", fns}});
  }
  out["files"] = files;

  return out;
}

} // namespace forkfinder

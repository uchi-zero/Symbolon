#include "StateKillerFinder.h"

#include <symbolon/Paths.h>

#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace statekillerfinder {

std::string_view categoryName(Category c) {
  switch (c) {
  case Category::ProgramTerminator:
    return "ProgramTerminator";
  case Category::UnreachableInstruction:
    return "UnreachableInstruction";
  case Category::UnsupportedInstruction:
    return "UnsupportedInstruction";
  case Category::UnimplementedIntrinsic:
    return "UnimplementedIntrinsic";
  case Category::ExternCallSymArg:
    return "ExternCallSymArg";
  case Category::ExternCallSymPtr:
    return "ExternCallSymPtr";
  case Category::ExternCallIndirect:
    return "ExternCallIndirect";
  case Category::InlineAsm:
    return "InlineAsm";
  case Category::IndirectBranch:
    return "IndirectBranch";
  case Category::SymbolicAllocSize:
    return "SymbolicAllocSize";
  case Category::SymbolicVectorIndex:
    return "SymbolicVectorIndex";
  case Category::CallArityMismatch:
    return "CallArityMismatch";
  case Category::FreeOfNonHeap:
    return "FreeOfNonHeap";
  }
  return "Unknown";
}

std::string_view severityName(Severity s) {
  switch (s) {
  case Severity::Definite:
    return "definite";
  case Severity::Likely:
    return "likely";
  case Severity::Conditional:
    return "conditional";
  }
  return "unknown";
}

Severity severityOf(Category c) {
  switch (c) {
  case Category::ProgramTerminator:
  case Category::UnreachableInstruction:
  case Category::UnsupportedInstruction:
  case Category::UnimplementedIntrinsic:
    return Severity::Definite;
  case Category::ExternCallSymArg:
  case Category::ExternCallSymPtr:
    return Severity::Likely;
  case Category::ExternCallIndirect:
    return Severity::Conditional;
  case Category::IndirectBranch:
    return Severity::Conditional;
  // NOTE: InlineAsm severity is pinned to --external-calls=concrete.
  // Under --external-calls=none it would be Definite.
  case Category::InlineAsm:
  case Category::SymbolicAllocSize:
    return Severity::Likely;
  case Category::SymbolicVectorIndex:
  case Category::CallArityMismatch:
  case Category::FreeOfNonHeap:
    return Severity::Definite;
  }
  return Severity::Definite;
}

namespace {

Severity severityForCategoryName(llvm::StringRef name) {
  static const llvm::StringMap<Severity> kMap = [] {
    llvm::StringMap<Severity> m;
    for (int i = 0; i < kCategoryCount; ++i) {
      Category c = static_cast<Category>(i);
      m[categoryName(c)] = severityOf(c);
    }
    return m;
  }();
  auto it = kMap.find(name);
  return it != kMap.end() ? it->second : Severity::Definite;
}

struct LineAgg {
  std::set<std::string> categories;
  std::set<std::string> opcodes;
  std::set<std::string> callees;
};

} // namespace

nlohmann::ordered_json renderReport(const std::vector<Finding> &findings,
                                    const Options &opts,
                                    std::string_view moduleName) {
  using json = nlohmann::ordered_json;

  std::map<std::string, std::map<std::string, std::map<unsigned, LineAgg>>>
      tree;
  std::map<std::string, std::size_t> by_category;
  std::map<std::string, std::size_t> by_severity;
  by_severity["definite"] = 0;
  by_severity["likely"] = 0;
  by_severity["conditional"] = 0;
  std::map<std::string, std::size_t> unlocated_by_category;
  std::map<std::string, std::size_t> unlocated_by_severity;
  std::map<std::string, std::size_t> system_skipped_by_category;
  std::map<std::string, std::size_t> system_skipped_by_severity;
  std::size_t total_located = 0;

  for (Category c : opts.enabled)
    by_category[std::string(categoryName(c))] = 0;

  for (const Finding &f : findings) {
    if (!(opts.enabled.count(f.category) > 0))
      continue;
    std::string cname(categoryName(f.category));
    std::string sname(severityName(severityOf(f.category)));
    if (f.file.empty()) {
      unlocated_by_category[cname] += 1;
      unlocated_by_severity[sname] += 1;
      continue;
    }
    if (!opts.include_system && symbolon::looksLikeSystemPath(f.file)) {
      system_skipped_by_category[cname] += 1;
      system_skipped_by_severity[sname] += 1;
      continue;
    }
    auto &agg = tree[f.file][f.function][f.line];
    bool line_was_empty = agg.categories.empty();
    bool new_category = agg.categories.insert(cname).second;
    agg.opcodes.insert(f.opcode);
    if (!f.callee.empty())
      agg.callees.insert(f.callee);
    if (new_category) {
      by_category[cname] += 1;
      by_severity[sname] += 1;
    }
    if (line_was_empty)
      total_located += 1;
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

  json bs = json::object();
  for (auto &kv : by_severity)
    bs[kv.first] = kv.second;
  summary["by_severity"] = bs;

  if (!unlocated_by_category.empty()) {
    json uc = json::object();
    for (auto &kv : unlocated_by_category)
      uc[kv.first] = kv.second;
    summary["unlocated_by_category"] = uc;
  }
  if (!unlocated_by_severity.empty()) {
    json us = json::object();
    for (auto &kv : unlocated_by_severity)
      us[kv.first] = kv.second;
    summary["unlocated_by_severity"] = us;
  }
  if (!system_skipped_by_category.empty()) {
    json sc = json::object();
    for (auto &kv : system_skipped_by_category)
      sc[kv.first] = kv.second;
    summary["system_skipped_by_category"] = sc;
  }
  if (!system_skipped_by_severity.empty()) {
    json ss = json::object();
    for (auto &kv : system_skipped_by_severity)
      ss[kv.first] = kv.second;
    summary["system_skipped_by_severity"] = ss;
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
        std::set<std::string> sev_set;
        for (auto &cname : lkv.second.categories)
          sev_set.insert(
              std::string(severityName(severityForCategoryName(cname))));
        entry["severities"] = json::array();
        for (auto &s : sev_set)
          entry["severities"].push_back(s);
        entry["opcodes"] = json::array();
        for (auto &o : lkv.second.opcodes)
          entry["opcodes"].push_back(o);
        entry["callees"] = json::array();
        for (auto &c : lkv.second.callees)
          entry["callees"].push_back(c);
        lines[std::to_string(lkv.first)] = entry;
      }
      fns[fnkv.first] = json::object({{"lines", lines}});
    }
    files[fkv.first] = json::object({{"functions", fns}});
  }
  out["files"] = files;

  return out;
}

} // namespace statekillerfinder

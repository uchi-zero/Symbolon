#include "symbolon/Paths.h"

namespace symbolon {

bool looksLikeSystemPath(std::string_view p) {
  if (p.substr(0, 5) == "/usr/")
    return true;
  if (p.substr(0, 11) == "/nix/store/")
    return true;
  static constexpr std::string_view kSubstrs[] = {
      "/klee/runtime/",
      "/klee-uclibc/",
      "/libcxx/",
      "/libcxxabi/",
  };
  for (auto s : kSubstrs)
    if (p.find(s) != std::string_view::npos)
      return true;
  return false;
}

std::string joinSourcePath(std::string_view directory,
                           std::string_view filename) {
  if (filename.empty())
    return {};
  if (filename.front() == '/')
    return std::string(filename);
  if (directory.empty())
    return std::string(filename);
  std::string out;
  out.reserve(directory.size() + 1 + filename.size());
  out.assign(directory);
  if (out.back() != '/')
    out.push_back('/');
  out.append(filename);
  return out;
}

} // namespace symbolon

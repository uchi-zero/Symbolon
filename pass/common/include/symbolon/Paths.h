#pragma once

#include <string>
#include <string_view>

namespace symbolon {

// Returns true if the path looks like a system header or KLEE runtime path
// that should be filtered out of findings by default. Used by all analysis
// passes to honor --include-system semantics consistently.
bool looksLikeSystemPath(std::string_view path);

// Joins a DIFile-style (directory, filename) pair into a single source path.
// Rules: empty filename => "". Absolute filename => filename verbatim (any
// directory is ignored). Empty directory => filename verbatim. Otherwise
// "<directory>/<filename>" with at most one separator.
std::string joinSourcePath(std::string_view directory,
                           std::string_view filename);

} // namespace symbolon

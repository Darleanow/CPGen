/**
 * @file PathResolver.cpp
 * @brief Implements the template directory resolution helpers.
 *
 * @author Darleanow
 */

#include "CPGen/System/PathResolver.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

namespace PathResolver {

std::filesystem::path executablePath() {
  std::vector<char> buf(512);

  while (true) {
    const ssize_t len =
        readlink("/proc/self/exe", buf.data(), buf.size() - 1);
    if (len == -1) {
      return {};
    }
    if (len < static_cast<ssize_t>(buf.size() - 1)) {
      return std::string(buf.data(), static_cast<std::size_t>(len));
    }
    buf.resize(buf.size() * 2);
  }
}

std::vector<std::filesystem::path> resolveTemplatePaths() {
  std::vector<std::filesystem::path> paths;

  for (const auto &candidate : {
           std::filesystem::path("/usr/local/share/cpgen/templates"),
           std::filesystem::path("/usr/share/cpgen/templates"),
       }) {
    if (std::filesystem::exists(candidate)) {
      paths.push_back(candidate);
    }
  }

  // Dev fallback: <exe_dir>/templates
  const auto exe = executablePath();
  if (!exe.empty()) {
    const auto dev_path = exe.parent_path() / "templates";
    if (std::filesystem::exists(dev_path)) {
      paths.push_back(dev_path);
    }
  }

  if (paths.empty()) {
    throw std::runtime_error(
        "No valid template directory found.  Searched system paths and "
        "executable-relative paths.");
  }

  return paths;
}

std::filesystem::path findTemplatesRoot() {
  return resolveTemplatePaths().front();
}

} // namespace PathResolver

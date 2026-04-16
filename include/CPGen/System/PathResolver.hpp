#pragma once

/**
 * @file PathResolver.hpp
 * @brief Shared utilities for locating CPGen's template directories.
 * @author Darleanow
 *
 * Centralises the template search-path logic that was previously duplicated
 * between @ref ModuleResolver and @c main.cpp.
 *
 * ### Search order
 * 1. @c ~/.config/cpgen/templates  (user-local)
 * 2. @c /usr/local/share/cpgen/templates  (system-wide, local install)
 * 3. @c /usr/share/cpgen/templates  (system-wide, package manager)
 * 4. @c \<executable_dir\>/templates  (development in-tree build)
 */

#include <filesystem>
#include <vector>

namespace PathResolver {

/**
 * @brief Resolve the absolute path of the running executable.
 *
 * Uses @c /proc/self/exe on Linux.  Returns an empty path if the symlink
 * cannot be read.
 *
 * @return Absolute path to the running executable, or empty on failure.
 */
[[nodiscard]] std::filesystem::path executablePath();

/**
 * @brief Build the ordered list of template search directories.
 *
 * Only directories that actually exist on disk are included.
 *
 * @return Non-empty vector of existing template root directories.
 * @throws std::runtime_error If no candidate directory is found.
 */
[[nodiscard]] std::vector<std::filesystem::path> resolveTemplatePaths();

/**
 * @brief Return the first existing template root directory.
 *
 * Convenience wrapper around @ref resolveTemplatePaths that returns
 * just the highest-priority path.
 *
 * @return Absolute path to the best template root.
 * @throws std::runtime_error If no candidate directory is found.
 */
[[nodiscard]] std::filesystem::path findTemplatesRoot();

} // namespace PathResolver

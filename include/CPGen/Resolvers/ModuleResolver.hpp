#pragma once

/**
 * @file ModuleResolver.hpp
 * @brief Looks up module JSON definitions on the filesystem and parses them.
 * @author Darleanow
 *
 * Module definitions are JSON files stored under a @c templates/modules/<name>/
 * directory.  The resolver searches a prioritised list of candidate directories
 * (system-wide, user-local, and next to the running executable for development)
 * until it finds the requested module.
 */

#include "CPGen/Core/Resolved.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

/**
 * @class ModuleResolver
 * @brief Resolves module names to their fully parsed @ref ResolvedModule
 * descriptors.
 *
 * On construction the resolver builds a list of candidate template directories
 * (see @ref resolvePaths).  Subsequent @ref resolveModule calls search those
 * directories in priority order and parse the first matching JSON file.
 *
 * ### Search path priority
 * 1. @c ~/.config/cpgen/templates  (user-local)
 * 2. @c /usr/local/share/cpgen/templates  (system-wide, local install)
 * 3. @c /usr/share/cpgen/templates  (system-wide, package manager install)
 * 4. @c <executable_dir>/templates  (development in-tree build)
 *
 * @throws std::runtime_error from the constructor if none of the candidate
 * paths exist on the current machine.
 */
class ModuleResolver {
public:
  /**
   * @brief Constructs a @c ModuleResolver and resolves template search paths.
   * @throws std::runtime_error If no valid template directory is found.
   */
  ModuleResolver();

  /**
   * @brief Locate and parse a module by name.
   *
   * Searches @ref m_resolved_paths for a file at
   * @c <path>/modules/<module_name>/<module_name>.json and parses it.
   *
   * @param module_name Identifier of the module to resolve (e.g. @c "gtest").
   * @return A pair of:
   *         - @ref ResolvedModule — parsed metadata and optional FetchContent
   * info.
   *         - @c std::vector<TargetInjection> — CMake snippets for each target
   * the module contributes to.
   * @throws std::runtime_error If the module JSON file cannot be found or
   * opened.
   */
  std::pair<ResolvedModule, std::vector<TargetInjection>>
  resolveModule(const std::string &module_name);

private:
  /**
   * @brief Populate @ref m_resolved_paths via @ref PathResolver.
   * @throws std::runtime_error If no candidate directory is found.
   */
  void resolvePaths();

  /**
   * @brief Read and parse a single module JSON file from @p path.
   * @param path Absolute path to the module's @c .json definition file.
   * @return Parsed @ref ResolvedModule and its @ref TargetInjection list.
   * @throws std::runtime_error If the file cannot be opened or the JSON is
   * malformed.
   */
  static std::pair<ResolvedModule, std::vector<TargetInjection>>
  loadModule(const std::filesystem::path &path);

  /**
   * @brief Extract all target injections from a parsed module JSON object.
   * @param json_obj The root JSON object of a module definition file.
   * @return Vector of @ref TargetInjection, one per entry in the @c "targets"
   * map.
   */
  static std::vector<TargetInjection>
  readInjection(const nlohmann::json &json_obj);

  std::vector<std::filesystem::path>
      m_resolved_paths; ///< Ordered list of template search directories.
};

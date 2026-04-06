#pragma once

/**
 * @file ProjectResolver.hpp
 * @brief Orchestrates the full dependency resolution pipeline for a project.
 * @author Darleanow
 *
 * Given a @ref ProjectConfig (produced by the CLI or TUI), @ref ProjectResolver
 * asks @ref ModuleResolver to resolve each requested dependency module, collects
 * all @ref TargetInjection objects, deduplicates the resulting build targets, and
 * returns a @ref ResolvedProject ready for the generators.
 */

#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Resolved.hpp"
#include "CPGen/Resolvers/ModuleResolver.hpp"
#include <memory>
#include <ostream>

/**
 * @class ProjectResolver
 * @brief Converts a @ref ProjectConfig into a fully resolved @ref ResolvedProject.
 *
 * Internally owns a @ref ModuleResolver and delegates per-module lookups to it.
 * Target deduplication is handled via @c std::unordered_set<ResolvedTarget>
 * (uses the name-based hash/equality defined in @ref Resolved.hpp).
 */
class ProjectResolver {
public:
  /**
   * @brief Construct a @c ProjectResolver with a default @ref ModuleResolver.
   * @throws std::runtime_error If the underlying @ref ModuleResolver cannot find
   *         any template search path.
   */
  ProjectResolver();

  /**
   * @brief Run the full resolution pipeline for @p config.
   *
   * For each module name in @c config.modules:
   * 1. Calls @ref ModuleResolver::resolveModule to parse the module's JSON.
   * 2. Calls @ref buildTarget for each @ref TargetInjection the module produces.
   * 3. Inserts deduplicated targets into the result.
   *
   * @param config Parsed project configuration from the CLI or TUI.
   * @return A @ref ResolvedProject containing the original config, all resolved
   *         modules, deduplicated targets, and raw injections.
   * @throws std::runtime_error If any requested module cannot be found.
   */
  ResolvedProject resolve(const ProjectConfig &config);

private:
  /**
   * @brief Build a single @ref ResolvedTarget from an injection and the project config.
   *
   * Determines the target type and source path based on the injection's
   * @c target_name (@c "test" → @ref TargetType::Test, @c "library" →
   * @ref TargetType::Library, everything else → @ref TargetType::Executable).
   *
   * @param injection The injection descriptor produced by @ref ModuleResolver.
   * @param config    The originating project configuration (used for path construction).
   * @return A fully populated @ref ResolvedTarget.
   */
  static ResolvedTarget buildTarget(const TargetInjection &injection,
                                    const ProjectConfig &config);

  std::unique_ptr<ModuleResolver> m_module_resolver; ///< Owned module resolver instance.
};

/**
 * @brief Debug stream operator for @ref ResolvedProject.
 *
 * Dumps a human-readable representation of the resolved project to @p os,
 * including config metadata, all modules (with optional FetchContent info),
 * and all generated targets.
 *
 * @param os   Output stream to write to.
 * @param proj The resolved project to dump.
 * @return @p os for chaining.
 *
 * @note This overload is intended for development/debugging only and will be
 *       removed once the generator pipeline is complete.
 */
inline std::ostream &operator<<(std::ostream &os, const ResolvedProject &proj) {
  os << "Begin dump of project config\n"
     << "Name: " << proj.config.name << "\nPath: " << proj.config.path;
  os << "\nBegin dump of modules: \n";

  for (const auto &mod : proj.modules) {
    os << "Name: " << mod.name;
    if (mod.fetch_content.has_value()) {
      os << "\nFetchContent name: " << mod.fetch_content->module_name
         << "\nFetchContent URL:" << mod.fetch_content->url;
    }
  }
  os << "\nBegin dump of deduced targets: \n";
  for (const auto &target : proj.targets) {
    os << "Name: " << target.name << "\nPath: " << target.path;
  }

  return os;
}

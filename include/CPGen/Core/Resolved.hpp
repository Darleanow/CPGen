#pragma once

/**
 * @file Resolved.hpp
 * @brief Output types produced by the resolver pipeline.
 *
 * After @ref ProjectResolver::resolve processes a @ref ProjectConfig, every
 * dependency module has been looked up on disk, its FetchContent block parsed,
 * and all CMake injections collected.  The resulting @ref ResolvedProject
 * aggregates all of these artefacts and is passed downstream to the generators.
 *
 * @author Darleanow
 */

#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Module.hpp"

/**
 * @struct TargetInjection
 * @brief Describes the CMake snippets that a module contributes to a target.
 *
 * Each module can inject link libraries, an optional @c include() directive,
 * and an optional post-link command (e.g. @c gtest_discover_tests) into one
 * or more named targets.
 */
struct TargetInjection {
  std::string
      target_name; ///< Name of the CMake target that receives this injection.
  std::string
      module_name; ///< Identifier of the module that produced this injection.
  std::vector<std::string>
      link; ///< Libraries to pass to @c target_link_libraries.

  /// Optional CMake module to @c include() (e.g. @c "GoogleTest").
  std::optional<std::string> cmake_include;

  /// Optional command appended after @c target_link_libraries (e.g. @c
  /// "gtest_discover_tests(${TARGET_NAME})").
  std::optional<std::string> post_link;
};

/**
 * @struct ResolvedModule
 * @brief A dependency module after its JSON definition has been parsed.
 *
 * Produced by @ref ModuleResolver::resolveModule for each requested module
 * name.
 */
struct ResolvedModule {
  std::string name; ///< Canonical module name as declared in its JSON file.

  /// FetchContent descriptor, present when the module ships its own source
  /// (downloaded at CMake configure time).
  std::optional<FetchContent> fetch_content;
};

/**
 * @struct ResolvedTarget
 * @brief A build target after module injections have been applied.
 *
 * Two @c ResolvedTarget objects are considered equal (for deduplication
 * purposes) when their @c name fields match, regardless of @c type or @c path.
 */
struct ResolvedTarget {
  TargetType type = TargetType::Executable; ///< CMake target kind.
  std::string name; ///< Target name used in CMake (e.g. @c "test").
  std::string path; ///< Relative source directory for this target.

  /**
   * @brief Equality based solely on the target name.
   * @param other The target to compare against.
   * @return @c true if both targets share the same name.
   */
  bool operator==(const ResolvedTarget &other) const {
    return name == other.name;
  }
};

/**
 * @brief @c std::hash specialisation for @ref ResolvedTarget.
 *
 * Enables use of @c ResolvedTarget in @c std::unordered_set / @c
 * std::unordered_map for target deduplication in @ref ProjectResolver::resolve.
 */
namespace std {
template <> struct hash<ResolvedTarget> {
  /**
   * @brief Computes a hash based on the target name.
   * @param target The target to hash.
   * @return Hash value derived from @c target.name.
   */
  size_t operator()(const ResolvedTarget &target) const noexcept {
    return hash<std::string>{}(target.name);
  }
};
} // namespace std

/**
 * @struct ResolvedProject
 * @brief Aggregated output of the full resolver pipeline.
 *
 * Returned by @ref ProjectResolver::resolve and consumed by the CMake and
 * C++ file generators to produce the final project structure on disk.
 */
struct ResolvedProject {
  ProjectConfig config; ///< The original configuration that was resolved.
  std::vector<ResolvedTarget>
      targets; ///< Deduplicated list of build targets to generate.
  std::vector<ResolvedModule> modules; ///< All resolved dependency modules.
  std::vector<TargetInjection>
      injections; ///< All CMake snippets collected from every module.
};

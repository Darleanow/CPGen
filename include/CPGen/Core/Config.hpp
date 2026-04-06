#pragma once

/**
 * @file Config.hpp
 * @brief Project configuration types produced by the CLI and consumed by the resolvers.
 * @author Darleanow
 */

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * @struct ProjectOptions
 * @brief Flat configuration bag used by the interactive TUI mode.
 *
 * Populated incrementally by the TUI widgets (Checkbox, Input) as the user
 * fills in the form.  This is separate from @ref ProjectConfig, which is the
 * typed output of the CLI flag parser.
 */
struct ProjectOptions {
  bool is_tui_mode = false; ///< Whether the application was launched in TUI mode.
  bool has_git     = false; ///< Whether to initialise a Git repository.

  std::string path; ///< Absolute path to the directory where the project will be generated.
  std::string name; ///< Name of the project (becomes the leaf directory name).

  bool use_template = false;                     ///< Whether to apply a named project template.
  std::optional<std::string> template_name;      ///< Template identifier, valid only when @c use_template is @c true.

  std::vector<std::string> modules_list; ///< Additional dependency modules requested by the user.
};

/**
 * @enum TargetType
 * @brief Describes the CMake target kind to generate for a build target.
 */
enum class TargetType : std::uint8_t {
  Library,    ///< A static or shared library target (@c add_library).
  Executable, ///< A standalone executable target (@c add_executable).
  Test,       ///< A test executable wired into CTest via a testing framework.
};

/**
 * @enum CppStandard
 * @brief Selects the C++ language standard to pass to the compiler.
 */
enum class CppStandard : std::uint8_t {
  Cpp17, ///< ISO C++17 (@c -std=c++17 / @c CMAKE_CXX_STANDARD 17).
  Cpp20, ///< ISO C++20 (@c -std=c++20 / @c CMAKE_CXX_STANDARD 20). Default.
  Cpp23, ///< ISO C++23 (@c -std=c++23 / @c CMAKE_CXX_STANDARD 23).
};

/**
 * @struct ToolingConfig
 * @brief Groups all optional tooling flags that can be enabled for a project.
 *
 * Each field corresponds to a CLI flag (e.g. @c --git, @c --clang-format).
 */
struct ToolingConfig {
  bool has_git       = false; ///< Initialise a Git repository in the project root.
  bool clang_tidy    = false; ///< Generate a @c .clang-tidy configuration file.
  bool clang_format  = false; ///< Generate a @c .clang-format configuration file.

  /// Clang-format style preset (e.g. @c "Google", @c "LLVM").
  /// Present only when @c clang_format is @c true and a preset was supplied.
  std::optional<std::string> clang_format_preset;
};

/**
 * @struct ProjectConfig
 * @brief Fully parsed project configuration produced by @ref CLI::parse.
 *
 * This struct is the primary input to @ref ProjectResolver::resolve and drives
 * the entire code-generation pipeline.
 */
struct ProjectConfig {
  std::string path;                    ///< Absolute path to the generation directory.
  std::string name;                    ///< Project name (used as the CMake project name and directory).
  CppStandard standard = CppStandard::Cpp20; ///< C++ language standard. Defaults to C++20.
  std::vector<std::string>  modules;   ///< Dependency module identifiers (e.g. @c "gtest", @c "spdlog").
  std::vector<TargetType>   targets;   ///< Build target kinds to generate.
  ToolingConfig             tooling;   ///< Optional tooling configuration.
};

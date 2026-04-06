#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief ProjectOptions has all the available base options, made to be
 * extensible using config files
 */
struct ProjectOptions {
  /*@{*/
  bool is_tui_mode = false; /**< Use TUI mode ? */
  bool has_git = false;     /**< Init git repo ? */
  std::string path;         /**< The target project path */
  std::string name;         /**< The name of the project (path+name/) */
  /*@}*/

  /**
   * @name Template parameters
   */
  /*@{*/
  bool use_template = false;                /**< Do we use a template ? */
  std::optional<std::string> template_name; /**< Name of the template if so */
  /*@}*/

  /*@{*/
  std::vector<std::string> modules_list; /** List of additional packages */
  /*@}*/
};

enum class TargetType : std::uint8_t { Library, Executable, Test };
enum class CppStandard : std::uint8_t { Cpp17, Cpp20, Cpp23 };

struct ToolingConfig {
  bool has_git = false;
  bool clang_tidy = false;
  std::optional<std::string> clang_format_preset;
  bool clang_format = false;
};

struct ProjectConfig {
  std::string path;
  std::string name;
  CppStandard standard = CppStandard::Cpp20;
  std::vector<std::string> modules;
  std::vector<TargetType> targets;
  ToolingConfig tooling;
};

#pragma once

/**
 * @file CLI.hpp
 * @brief CLI Operations
 * @author Darleanow
 * @version 1.0.1
 */

#include <optional>
#include <string>
#include <unordered_map>

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
};

/**
 * @class CLI
 * @brief Parse various opts
 */
class CLI {
public:
  /**
   * @brief Parse argc, argv
   * @param argc: Argument count
   * @param argv: Argument list
   * @return The options parsed
   */
  const ProjectOptions parse(const int argc, char **argv);

  /**
   * @brief Translates ProjectOptions into a keymap
   * @param opts: The previously parsed CLIOpts
   * @return Unordered map of keys and values.
   */
  static std::unordered_map<std::string, std::string>
  parse_opts(const ProjectOptions &opts);
};

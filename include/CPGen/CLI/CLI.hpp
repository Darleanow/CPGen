#pragma once

/**
 * @file CLI.hpp
 * @brief CLI Operations
 * @author Darleanow
 * @version 1.0.1
 */

#include "CPGen/Core/Config.hpp"
#include <string>
#include <unordered_map>
#include <variant>

using Config = std::variant<ProjectConfig, bool>;

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
  Config parse(const int argc, char **argv);

  /**
   * @brief Translates ProjectOptions into a keymap
   * @param opts: The previously parsed CLIOpts
   * @return Unordered map of keys and values.
   */
  static std::unordered_map<std::string, std::string>
  parse_opts(const ProjectOptions &opts);
};

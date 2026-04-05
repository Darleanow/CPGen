#pragma once

/**
 * @file CLI.hpp
 * @brief CLI Operations
 * @author Darleanow
 * @version 1.0.1
 */

#include "CPGen/Core/Config.hpp"
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

private:
  /**
   * @brief Shows all register command line args
   */
  void showHelp() const;
};

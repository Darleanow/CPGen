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
 * @brief Parses command-line arguments into a project configuration.
 *
 * Usage:
 *   CLI cli;
 *   auto config = cli.parse(argc, argv);
 */
class CLI {
public:
  Config parse(int argc, char **argv);

private:
  void parseName(int argc, char **argv, int &i);
  void parsePath(int argc, char **argv, int &i);
  void parseStandard(int argc, char **argv, int &i);
  void parseModules(int argc, char **argv, int &i);
  void parseTargets(int argc, char **argv, int &i);
  void parseClangFormatPreset(int argc, char **argv, int &i);
  void parseProjectFlag(int argc, char **argv, int &i);
  void parseToolingFlag(int argc, char **argv, int &i);

  static void showHelp();

  ProjectConfig m_config;
};

#pragma once

/**
 * @file CLI.hpp
 * @brief Command-line argument parser that produces a typed project configuration.
 * @author Darleanow
 * @version 1.0.1
 *
 * Typical usage:
 * @code
 *   CLI cli;
 *   auto cfg = cli.parse(argc, argv);
 *   if (std::holds_alternative<ProjectConfig>(cfg)) {
 *     // non-interactive generation path
 *   } else {
 *     // launch TUI
 *   }
 * @endcode
 */

#include "CPGen/Core/Config.hpp"
#include <variant>

/**
 * @typedef Config
 * @brief Result type of @ref CLI::parse.
 *
 * - @c ProjectConfig — when the user supplied project flags; the caller should
 *   run the resolver and generators immediately.
 * - @c bool (@c true) — when @c -u / @c --tui was supplied; the caller should
 *   launch the interactive TUI instead.
 */
using Config = std::variant<ProjectConfig, bool>;

/**
 * @class CLI
 * @brief Stateful command-line parser that converts @c argv into a @ref Config.
 *
 * Each @ref parse call resets the internal @ref ProjectConfig accumulator, so a
 * single @c CLI instance can safely be reused across multiple test invocations.
 *
 * Supported flags:
 * | Short | Long              | Description                               |
 * |-------|-------------------|-------------------------------------------|
 * | @c -h | @c --help         | Print usage and exit.                     |
 * | @c -u | @c --tui          | Return @c true to trigger TUI mode.       |
 * | @c -n | @c --name         | Project name (must match @c ^\w+$).       |
 * | @c -p | @c --path         | Target directory (must exist on disk).    |
 * | @c -s | @c --std          | C++ standard: @c 17, @c 20, or @c 23.    |
 * | @c -m | @c --modules      | Comma-separated dependency modules.       |
 * | @c -tar | @c --targets    | Comma-separated target types.             |
 * | @c -g | @c --git          | Enable Git initialisation.                |
 * | @c -cf | @c --clang-format | Enable clang-format file generation.     |
 * | @c -cfp | @c --cf-preset  | clang-format style preset name.          |
 * | @c -ct | @c --clang-tidy  | Enable clang-tidy file generation.       |
 */
class CLI {
public:
  /**
   * @brief Parse command-line arguments into a typed configuration.
   *
   * Iterates over @p argv entries in order and dispatches each recognised flag
   * to the appropriate private helper.  Unknown flags are silently skipped.
   *
   * @param argc Number of argument strings in @p argv.
   * @param argv Null-terminated array of argument strings (standard C @c main signature).
   * @return A @ref ProjectConfig if project flags were parsed, or @c true if
   *         @c --tui was requested.
   * @throws std::runtime_error When a required flag argument is missing, the path
   *         does not exist, or the name does not match the required pattern.
   * @throws std::range_error   When the C++ standard value is not 17, 20, or 23.
   */
  Config parse(int argc, char **argv);

private:
  /**
   * @brief Parse the @c -n / @c --name flag and advance @p i.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index; incremented to consume the flag's value.
   * @throws std::runtime_error If no value follows the flag or the value starts
   *         with @c '-', or if the value does not match @c ^\\w+$.
   */
  void parseName(int argc, char **argv, int &i);

  /**
   * @brief Parse the @c -p / @c --path flag and advance @p i.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index; incremented to consume the flag's value.
   * @throws std::runtime_error If no value follows the flag, the value starts with
   *         @c '-', or the path does not exist on the filesystem.
   */
  void parsePath(int argc, char **argv, int &i);

  /**
   * @brief Parse the @c -s / @c --std flag and advance @p i.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index; incremented to consume the flag's value.
   * @throws std::runtime_error If no value follows the flag.
   * @throws std::range_error   If the value is not @c "17", @c "20", or @c "23".
   */
  void parseStandard(int argc, char **argv, int &i);

  /**
   * @brief Parse the @c -m / @c --modules flag and advance @p i.
   *
   * Accepts a comma-separated list such as @c "gtest,spdlog".
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index; incremented to consume the flag's value.
   * @throws std::runtime_error If no value follows the flag.
   */
  void parseModules(int argc, char **argv, int &i);

  /**
   * @brief Parse the @c -tar / @c --targets flag and advance @p i.
   *
   * Recognises @c "exe" / @c "executable" → @ref TargetType::Executable,
   * @c "test" → @ref TargetType::Test, and anything else → @ref TargetType::Library.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index; incremented to consume the flag's value.
   * @throws std::runtime_error If no value follows the flag.
   */
  void parseTargets(int argc, char **argv, int &i);

  /**
   * @brief Parse the @c -cfp / @c --cf-preset flag and advance @p i.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index; incremented to consume the flag's value.
   * @throws std::runtime_error If no value follows the flag.
   */
  void parseClangFormatPreset(int argc, char **argv, int &i);

  /**
   * @brief Dispatch project-related flags (@c -n, @c -p, @c -s, @c -m, @c -tar) for index @p i.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index (not advanced here; individual helpers advance it).
   */
  void parseProjectFlag(int argc, char **argv, int &i);

  /**
   * @brief Dispatch tooling flags (@c -g, @c -cf, @c -cfp, @c -ct) for index @p i.
   * @param argc Total argument count.
   * @param argv Argument array.
   * @param i    Current index (not advanced here; individual helpers advance it).
   */
  void parseToolingFlag(int argc, char **argv, int &i);

  /**
   * @brief Print the full help text to @c stdout and call @c std::_Exit(0).
   */
  static void showHelp();

  ProjectConfig m_config; ///< Accumulates parsed values across helper calls within a single @ref parse invocation.
};

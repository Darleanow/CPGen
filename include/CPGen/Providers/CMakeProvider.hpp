#pragma once

/**
 * @file CMakeProvider.hpp
 * @brief Factory of CMake script snippet strings used by the generator
 * pipeline.
 * @author Darleanow
 *
 * @ref CMakeProvider is a pure static factory: every method returns a
 * well-formed CMake fragment string with no side effects.  The generator
 * assembles these fragments in the right order to produce a complete
 * @c CMakeLists.txt file.
 *
 * All returned strings are ready to be written directly to a file — they
 * contain newlines where appropriate but do **not** end with a trailing
 * newline themselves.
 */

#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Module.hpp"
#include <filesystem>
#include <string>
#include <vector>

/**
 * @class CMakeProvider
 * @brief Static factory that generates individual CMake script snippets.
 *
 * Each method corresponds to one logical CMake construct.  The caller is
 * responsible for ordering the snippets correctly in the output file.
 *
 * ### Typical assembly order
 * 1. @ref getMinimumRequired
 * 2. @ref getProjectHeader
 * 3. @ref getStandard
 * 4. @ref getIncludeFetchContent  *(if any FetchContent modules)*
 * 5. @ref getDeclareModule        *(once per module)*
 * 6. @ref getMakeAvailable        *(once, with all module names)*
 * 7. @ref getCompileOptions
 * 8. @ref getClangTidyTooling     *(if clang-tidy is enabled)*
 * 9. @ref getTargetDeclaration    *(once per target)*
 * 10. @ref getIncludeDirectories  *(once per target)*
 * 11. @ref getTargetLink          *(once per target that has dependencies)*
 * 12. @ref getEnableTests         *(if a test target is present)*
 */
class CMakeProvider {
public:
  /**
   * @brief Generate a @c cmake_minimum_required() call.
   *
   * Uses the @c {:g} format specifier so trailing zeros are omitted
   * (e.g. @c 3.0 → @c "3", @c 3.31 → @c "3.31").
   *
   * @param version Minimum required CMake version (e.g. @c 3.31f).
   * @return @c "cmake_minimum_required(VERSION <version>)"
   */
  [[nodiscard]] static std::string getMinimumRequired(float version);

  /**
   * @brief Generate a @c project() block with a fixed @c VERSION and @c CXX
   * language.
   *
   * @param name CMake project name (used verbatim).
   * @return Multi-line @c project() declaration string.
   */
  [[nodiscard]] static std::string getProjectHeader(const std::string &name);

  /**
   * @brief Generate @c CMAKE_CXX_STANDARD and @c CMAKE_CXX_STANDARD_REQUIRED
   * settings.
   *
   * @param standard The C++ standard to use.
   * @return Two-line @c set() block.
   *
   * @see toString (private helper)
   */
  [[nodiscard]] static std::string getStandard(CppStandard standard);

  /**
   * @brief Generate @c include(FetchContent).
   *
   * Must appear before any @ref getDeclareModule / @ref getMakeAvailable call.
   *
   * @return @c "include(FetchContent)"
   */
  [[nodiscard]] static std::string getIncludeFetchContent();

  /**
   * @brief Generate a @c FetchContent_Declare() block for a single module.
   *
   * @param fetch_content The module's FetchContent descriptor
   *        (@ref FetchContent::module_name and @ref FetchContent::url).
   * @return Multi-line @c FetchContent_Declare() string.
   */
  [[nodiscard]] static std::string
  getDeclareModule(const FetchContent &fetch_content);

  /**
   * @brief Generate a @c FetchContent_MakeAvailable() call for a list of
   * modules.
   *
   * Module names are space-separated on a single indented line.
   *
   * @param modules Non-empty list of module names to make available.
   * @return Multi-line @c FetchContent_MakeAvailable() string.
   * @pre @p modules must not be empty.
   */
  [[nodiscard]] static std::string
  getMakeAvailable(const std::vector<std::string> &modules);

  /**
   * @brief Generate the project-wide @c add_compile_options() call.
   *
   * Includes the standard warning flags used by the project
   * (@c -Wall @c -Wextra @c -Wpedantic @c -Wshadow @c -Wconversion @c -Werror).
   *
   * @return @c add_compile_options(...) string.
   */
  [[nodiscard]] static std::string getCompileOptions();

  /**
   * @brief Generate the clang-tidy integration block.
   *
   * Produces a @c find_program / @c if / @c set block that locates
   * @c clang-tidy and wires it into CMake's @c CMAKE_CXX_CLANG_TIDY variable
   * with a header filter covering @c include/ and @c src/.
   *
   * @return Multi-line clang-tidy CMake block.
   */
  [[nodiscard]] static std::string getClangTidyTooling();

  /**
   * @brief Generate an @c add_executable() or @c add_library() declaration.
   *
   * Source file paths are made relative by stripping everything before the
   * first occurrence of @p project_name in the absolute path.
   *
   * @param project_name Used to compute relative file paths.
   * @param type         @ref TargetType::Library → @c add_library,
   *                     anything else → @c add_executable.
   * @param target_name  CMake target name.
   * @param files        List of source file paths to include in the target.
   * @return Multi-line @c add_executable() / @c add_library() string.
   * @pre @p files must not be empty.
   */
  [[nodiscard]] static std::string
  getTargetDeclaration(const std::string &project_name, TargetType type,
                       const std::string &target_name,
                       const std::vector<std::filesystem::path> &files);

  /**
   * @brief Generate a @c target_include_directories() call with @c PUBLIC
   * scope.
   *
   * Always points at @c ${CMAKE_CURRENT_SOURCE_DIR}/include.
   *
   * @param target_name The CMake target to configure.
   * @return @c target_include_directories() string.
   */
  [[nodiscard]] static std::string
  getIncludeDirectories(const std::string &target_name);

  /**
   * @brief Generate a @c target_link_libraries() call with @c PUBLIC scope.
   *
   * Libraries are space-separated on a single line.
   *
   * @param target_name The CMake target to link.
   * @param libs        Non-empty list of library names / CMake targets to link
   * against.
   * @return @c target_link_libraries() string.
   * @pre @p libs must not be empty.
   *
   * @todo Add per-library scope configuration (PUBLIC / PRIVATE / INTERFACE).
   */
  [[nodiscard]] static std::string
  getTargetLink(const std::string &target_name,
                const std::vector<std::string> &libs);

  /**
   * @brief Generate @c enable_testing().
   *
   * Must be called before any @c add_test() / @c gtest_discover_tests() usage.
   *
   * @return @c "enable_testing()"
   */
  [[nodiscard]] static std::string getEnableTests();

private:
  /**
   * @brief Convert a @ref CppStandard enum value to its numeric string.
   * @param standard The C++ standard to convert.
   * @return @c "17", @c "20", or @c "23".
   */
  [[nodiscard]] static std::string toString(CppStandard standard) noexcept;
};

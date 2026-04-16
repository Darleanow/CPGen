#pragma once

/**
 * @file CMakeEmitter.hpp
 * @brief Emitter that produces a complete CMakeLists.txt for a generated
 * project.
 * @author Darleanow
 *
 * Absorbs the fragment-generation logic previously housed in @c CMakeProvider
 * and adds orchestration to assemble a complete, ready-to-use build file.
 */

#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Module.hpp"
#include "CPGen/Pipeline/Emitter.hpp"

#include <string>
#include <vector>

/**
 * @class CMakeEmitter
 * @brief Generates a fully assembled CMakeLists.txt from a @ref
 * ResolvedProject.
 *
 * ### Fragment order
 * 1. @c cmake_minimum_required
 * 2. @c project()
 * 3. C++ standard settings
 * 4. @c FetchContent declarations (if modules are present)
 * 5. Compile options
 * 6. Clang-tidy integration (if enabled)
 * 7. Target declarations (executables, libraries)
 * 8. Include directories
 * 9. Link libraries per target (from module injections)
 * 10. @c enable_testing() (if a test target is present)
 */
class CMakeEmitter : public Emitter {
public:
  [[nodiscard]] std::vector<OutputFile>
  emit(const ResolvedProject &project) override;

private:
  /// @name CMake Fragment Generators
  /// @{
  [[nodiscard]] static std::string minimumRequired(float version);
  [[nodiscard]] static std::string projectHeader(const std::string &name);
  [[nodiscard]] static std::string cxxStandard(CppStandard standard);
  [[nodiscard]] static std::string fetchContentInclude();
  [[nodiscard]] static std::string declareModule(const FetchContent &fc);
  [[nodiscard]] static std::string
  makeAvailable(const std::vector<std::string> &modules);
  [[nodiscard]] static std::string compileOptions();
  [[nodiscard]] static std::string clangTidyBlock();
  [[nodiscard]] static std::string
  targetDeclaration(TargetType type, const std::string &target_name,
                    const std::vector<std::string> &sources);
  [[nodiscard]] static std::string
  includeDirectories(const std::string &target_name);
  [[nodiscard]] static std::string
  targetLink(const std::string &target_name,
             const std::vector<std::string> &libs);
  [[nodiscard]] static std::string enableTesting();
  [[nodiscard]] static std::string
  cppStandardString(CppStandard standard) noexcept;
  /// @}

  /**
   * @brief Assemble all fragments into a complete CMakeLists.txt.
   * @param project Resolved project to generate for.
   * @return Complete CMakeLists.txt content.
   */
  [[nodiscard]] static std::string assemble(const ResolvedProject &project);

  /**
   * @brief Emit CMake link/include/post-link blocks for a single target.
   * @param injections All injections from the resolved project.
   * @param target_name The logical target name to match injections against.
   * @param cmake_name The CMake target name to use in the output.
   * @return CMake fragment string for the matched injections.
   */
  [[nodiscard]] static std::string
  applyInjections(const std::vector<TargetInjection> &injections,
                  const std::string &target_name,
                  const std::string &cmake_name);

  /**
   * @brief Derive a CMake-friendly target name from a resolved target.
   * @param target The resolved target.
   * @param project_name The project name (used for the main executable).
   * @return A suitable CMake target name.
   */
  [[nodiscard]] static std::string
  cmakeTargetName(const ResolvedTarget &target,
                  const std::string &project_name);

  /**
   * @brief Derive conventional source file paths for a given target.
   * @param target The resolved target.
   * @return List of source file paths relative to the project root.
   */
  [[nodiscard]] static std::vector<std::string>
  conventionalSources(const ResolvedTarget &target);
};

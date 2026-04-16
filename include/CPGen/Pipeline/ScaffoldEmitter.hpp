#pragma once

/**
 * @file ScaffoldEmitter.hpp
 * @brief Emitter that produces the project's directory scaffold and starter
 * source files.
 * @author Darleanow
 */

#include "CPGen/Pipeline/Emitter.hpp"

#include <filesystem>

/**
 * @class ScaffoldEmitter
 * @brief Generates starter source files for a new C++ project.
 *
 * Produces:
 * - @c src/main.cpp — hello-world entry point.
 * - @c tests/test_main.cpp — basic test skeleton (when a test target exists).
 * - @c .gitignore — standard C++ ignores (when Git is enabled).
 * - @c README.md — minimal project readme.
 */
class ScaffoldEmitter : public Emitter {
public:
  /**
   * @brief Construct with a path to the templates root directory.
   * @param template_root Absolute path to the @c templates/ directory
   *        (used for module example files such as @c gtest/example.cpp).
   */
  explicit ScaffoldEmitter(std::filesystem::path template_root);

  [[nodiscard]] std::vector<OutputFile>
  emit(const ResolvedProject &project) override;

private:
  [[nodiscard]] static std::string
  generateMainCpp(const std::string &project_name);
  [[nodiscard]] static std::string generateGitignore();
  [[nodiscard]] static std::string
  generateReadme(const std::string &project_name);
  [[nodiscard]] std::string generateTestMain(const ResolvedProject &project);

  std::filesystem::path m_template_root;
};

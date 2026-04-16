#pragma once

/**
 * @file ToolingEmitter.hpp
 * @brief Emitter that produces tooling configuration files (.clang-format,
 * .clang-tidy).
 * @author Darleanow
 */

#include "CPGen/Pipeline/Emitter.hpp"

#include <filesystem>

/**
 * @class ToolingEmitter
 * @brief Copies tooling configuration files from @c templates/features/ into
 * the generated project.
 *
 * Reads static config files from disk and emits them as @ref OutputFile
 * entries. Only emits files for tooling options that were enabled in the
 * project configuration.
 */
class ToolingEmitter : public Emitter {
public:
  /**
   * @brief Construct with a path to the templates root directory.
   * @param template_root Absolute path to the @c templates/ directory.
   */
  explicit ToolingEmitter(std::filesystem::path template_root);

  [[nodiscard]] std::vector<OutputFile>
  emit(const ResolvedProject &project) override;

private:
  /**
   * @brief Read the full contents of a file into a string.
   * @param path Absolute path to the file.
   * @return File contents, or an empty string if the file cannot be read.
   */
  [[nodiscard]] static std::string
  readFile(const std::filesystem::path &path);

  std::filesystem::path m_template_root;
};

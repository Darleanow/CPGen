/**
 * @file ToolingEmitter.cpp
 * @brief Implements the tooling configuration emitter.
 *
 * Reads static configuration files (.clang-tidy, .clang-format) from the
 * @c templates/features/ directory and emits them into the generated project
 * when the corresponding tooling option is enabled.
 *
 * @author Darleanow
 */

#include "CPGen/Pipeline/ToolingEmitter.hpp"

#include <fstream>
#include <sstream>
#include <utility>

ToolingEmitter::ToolingEmitter(std::filesystem::path template_root)
    : m_template_root(std::move(template_root)) {}

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

std::vector<OutputFile>
ToolingEmitter::emit(const ResolvedProject &project) {
  std::vector<OutputFile> files;

  if (project.config.tooling.clang_tidy) {
    const auto src =
        m_template_root / "features" / "clang-tidy" / ".clang-tidy";
    auto content = readFile(src);
    if (!content.empty()) {
      files.push_back({".clang-tidy", std::move(content)});
    }
  }

  if (project.config.tooling.clang_format) {
    const auto src =
        m_template_root / "features" / "clang-format" / ".clang-format";
    auto content = readFile(src);
    if (!content.empty()) {
      files.push_back({".clang-format", std::move(content)});
    }
  }

  return files;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

std::string ToolingEmitter::readFile(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    return {};
  }
  std::ifstream file(path);
  if (!file.is_open()) {
    return {};
  }
  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

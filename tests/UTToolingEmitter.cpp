/**
 * @file UTToolingEmitter.cpp
 * @brief Exhaustive unit tests for @ref ToolingEmitter.
 *
 * Tests cover both clang-tidy and clang-format emission, including the case
 * where template files are missing from disk.
 */

#include "CPGen/Pipeline/ToolingEmitter.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Resolved.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

// ============================================================================
// Helpers
// ============================================================================

namespace {

const std::filesystem::path kNonexistentRoot{"/tmp/cpgen_nonexistent_root"};

ResolvedProject makeMinimalProject() {
  ResolvedProject project;
  project.config.name = "ToolTest";
  project.config.path = "/tmp/";
  return project;
}

const OutputFile *findFile(const std::vector<OutputFile> &files,
                           const std::filesystem::path &rel) {
  auto it = std::find_if(
      files.begin(), files.end(),
      [&rel](const OutputFile &f) { return f.relative_path == rel; });
  return it != files.end() ? &(*it) : nullptr;
}

/// @brief RAII helper that creates a temporary directory tree with fake
///        feature template files, and removes it on destruction.
class TempTemplateTree {
public:
  TempTemplateTree() {
    m_root = std::filesystem::temp_directory_path() / "cpgen_test_templates";
    std::filesystem::create_directories(m_root / "features" / "clang-tidy");
    std::filesystem::create_directories(m_root / "features" / "clang-format");

    {
      std::ofstream f(m_root / "features" / "clang-tidy" / ".clang-tidy");
      f << "Checks: '-*'\n";
    }
    {
      std::ofstream f(m_root / "features" / "clang-format" / ".clang-format");
      f << "BasedOnStyle: Google\n";
    }
  }

  ~TempTemplateTree() {
    std::filesystem::remove_all(m_root);
  }

  TempTemplateTree(const TempTemplateTree &) = delete;
  TempTemplateTree &operator=(const TempTemplateTree &) = delete;
  TempTemplateTree(TempTemplateTree &&) = delete;
  TempTemplateTree &operator=(TempTemplateTree &&) = delete;

  [[nodiscard]] const std::filesystem::path &root() const { return m_root; }

private:
  std::filesystem::path m_root;
};

} // namespace

// ============================================================================
// Nothing enabled — empty output
// ============================================================================

TEST(UTToolingEmitter, NothingEnabledProducesNoFiles) {
  ToolingEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  EXPECT_TRUE(files.empty());
}

// ============================================================================
// clang-tidy
// ============================================================================

TEST(UTToolingEmitter, ClangTidyEnabledWithValidRoot) {
  TempTemplateTree tree;
  auto project = makeMinimalProject();
  project.config.tooling.clang_tidy = true;

  ToolingEmitter emitter(tree.root());
  auto files = emitter.emit(project);
  EXPECT_NE(findFile(files, ".clang-tidy"), nullptr);
}

TEST(UTToolingEmitter, ClangTidyContentMatchesTemplate) {
  TempTemplateTree tree;
  auto project = makeMinimalProject();
  project.config.tooling.clang_tidy = true;

  ToolingEmitter emitter(tree.root());
  auto files = emitter.emit(project);
  const auto *f = findFile(files, ".clang-tidy");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("Checks:"), std::string::npos);
}

TEST(UTToolingEmitter, ClangTidyEnabledMissingFileProducesNothing) {
  auto project = makeMinimalProject();
  project.config.tooling.clang_tidy = true;

  // Nonexistent root → readFile returns empty → no file emitted
  ToolingEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  EXPECT_EQ(findFile(files, ".clang-tidy"), nullptr);
}

// ============================================================================
// clang-format
// ============================================================================

TEST(UTToolingEmitter, ClangFormatEnabledWithValidRoot) {
  TempTemplateTree tree;
  auto project = makeMinimalProject();
  project.config.tooling.clang_format = true;

  ToolingEmitter emitter(tree.root());
  auto files = emitter.emit(project);
  EXPECT_NE(findFile(files, ".clang-format"), nullptr);
}

TEST(UTToolingEmitter, ClangFormatContentMatchesTemplate) {
  TempTemplateTree tree;
  auto project = makeMinimalProject();
  project.config.tooling.clang_format = true;

  ToolingEmitter emitter(tree.root());
  auto files = emitter.emit(project);
  const auto *f = findFile(files, ".clang-format");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("BasedOnStyle"), std::string::npos);
}

TEST(UTToolingEmitter, ClangFormatEnabledMissingFileProducesNothing) {
  auto project = makeMinimalProject();
  project.config.tooling.clang_format = true;

  ToolingEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  EXPECT_EQ(findFile(files, ".clang-format"), nullptr);
}

// ============================================================================
// Both enabled
// ============================================================================

TEST(UTToolingEmitter, BothEnabledProducesTwoFiles) {
  TempTemplateTree tree;
  auto project = makeMinimalProject();
  project.config.tooling.clang_tidy = true;
  project.config.tooling.clang_format = true;

  ToolingEmitter emitter(tree.root());
  auto files = emitter.emit(project);
  EXPECT_EQ(files.size(), 2U);
}

// ============================================================================
// Only one enabled
// ============================================================================

TEST(UTToolingEmitter, OnlyClangTidyProducesOneFile) {
  TempTemplateTree tree;
  auto project = makeMinimalProject();
  project.config.tooling.clang_tidy = true;
  project.config.tooling.clang_format = false;

  ToolingEmitter emitter(tree.root());
  auto files = emitter.emit(project);
  EXPECT_EQ(files.size(), 1U);
  EXPECT_NE(findFile(files, ".clang-tidy"), nullptr);
}

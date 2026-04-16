/**
 * @file UTScaffoldEmitter.cpp
 * @brief Exhaustive unit tests for @ref ScaffoldEmitter.
 *
 * Tests cover main.cpp generation, README, .gitignore conditionality,
 * test skeleton generation, and module example file loading.
 */

#include "CPGen/Pipeline/ScaffoldEmitter.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Resolved.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <string>

// ============================================================================
// Helpers
// ============================================================================

namespace {

/// @brief Returns a path that definitely doesn't contain module templates.
const std::filesystem::path kNonexistentRoot{"/tmp/cpgen_nonexistent_root"};

ResolvedProject makeMinimalProject() {
  ResolvedProject project;
  project.config.name = "FooApp";
  project.config.path = "/tmp/";
  project.config.standard = CppStandard::Cpp20;
  return project;
}

/// @brief Find an OutputFile by relative path.
const OutputFile *findFile(const std::vector<OutputFile> &files,
                           const std::filesystem::path &rel) {
  auto it = std::find_if(
      files.begin(), files.end(),
      [&rel](const OutputFile &f) { return f.relative_path == rel; });
  return it != files.end() ? &(*it) : nullptr;
}

} // namespace

// ============================================================================
// Always-produced files
// ============================================================================

TEST(UTScaffoldEmitter, AlwaysProducesMainCpp) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  EXPECT_NE(findFile(files, "src/main.cpp"), nullptr);
}

TEST(UTScaffoldEmitter, AlwaysProducesReadme) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  EXPECT_NE(findFile(files, "README.md"), nullptr);
}

// ============================================================================
// main.cpp content
// ============================================================================

TEST(UTScaffoldEmitter, MainCppContainsProjectName) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  const auto *f = findFile(files, "src/main.cpp");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("FooApp"), std::string::npos);
}

TEST(UTScaffoldEmitter, MainCppContainsIostream) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  const auto *f = findFile(files, "src/main.cpp");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("#include <iostream>"), std::string::npos);
}

TEST(UTScaffoldEmitter, MainCppContainsMainFunction) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  const auto *f = findFile(files, "src/main.cpp");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("int main()"), std::string::npos);
}

// ============================================================================
// README content
// ============================================================================

TEST(UTScaffoldEmitter, ReadmeContainsProjectName) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  const auto *f = findFile(files, "README.md");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("# FooApp"), std::string::npos);
}

TEST(UTScaffoldEmitter, ReadmeContainsCPGenLink) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  const auto *f = findFile(files, "README.md");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("CPGen"), std::string::npos);
}

// ============================================================================
// .gitignore conditionality
// ============================================================================

TEST(UTScaffoldEmitter, GitDisabledNoGitignore) {
  auto project = makeMinimalProject();
  project.config.tooling.has_git = false;
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  EXPECT_EQ(findFile(files, ".gitignore"), nullptr);
}

TEST(UTScaffoldEmitter, GitEnabledProducesGitignore) {
  auto project = makeMinimalProject();
  project.config.tooling.has_git = true;
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  EXPECT_NE(findFile(files, ".gitignore"), nullptr);
}

TEST(UTScaffoldEmitter, GitignoreContainsBuild) {
  auto project = makeMinimalProject();
  project.config.tooling.has_git = true;
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  const auto *f = findFile(files, ".gitignore");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("build/"), std::string::npos);
}

TEST(UTScaffoldEmitter, GitignoreContainsObjectFiles) {
  auto project = makeMinimalProject();
  project.config.tooling.has_git = true;
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  const auto *f = findFile(files, ".gitignore");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("*.o"), std::string::npos);
}

// ============================================================================
// Test file — no test target
// ============================================================================

TEST(UTScaffoldEmitter, NoTestTargetNoTestFile) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  EXPECT_EQ(findFile(files, "tests/test_main.cpp"), nullptr);
}

// ============================================================================
// Test file — with test target
// ============================================================================

TEST(UTScaffoldEmitter, TestTargetProducesTestFile) {
  auto project = makeMinimalProject();
  ResolvedTarget target;
  target.type = TargetType::Test;
  target.name = "test";
  project.targets.push_back(target);

  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  EXPECT_NE(findFile(files, "tests/test_main.cpp"), nullptr);
}

TEST(UTScaffoldEmitter, TestFileFallbackContainsGtest) {
  auto project = makeMinimalProject();
  ResolvedTarget target;
  target.type = TargetType::Test;
  target.name = "test";
  project.targets.push_back(target);
  // No injections → no module example file → uses fallback

  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  const auto *f = findFile(files, "tests/test_main.cpp");
  ASSERT_NE(f, nullptr);
  EXPECT_NE(f->content.find("gtest/gtest.h"), std::string::npos);
  EXPECT_NE(f->content.find("EXPECT_EQ"), std::string::npos);
}

// ============================================================================
// File count
// ============================================================================

TEST(UTScaffoldEmitter, MinimalProjectProducesTwoFiles) {
  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(makeMinimalProject());
  // main.cpp + README.md
  EXPECT_EQ(files.size(), 2U);
}

TEST(UTScaffoldEmitter, FullProjectProducesFourFiles) {
  auto project = makeMinimalProject();
  project.config.tooling.has_git = true;
  ResolvedTarget target;
  target.type = TargetType::Test;
  target.name = "test";
  project.targets.push_back(target);

  ScaffoldEmitter emitter(kNonexistentRoot);
  auto files = emitter.emit(project);
  // main.cpp + README.md + .gitignore + test_main.cpp
  EXPECT_EQ(files.size(), 4U);
}

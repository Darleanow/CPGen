/**
 * @file UTGenerationPipeline.cpp
 * @brief Exhaustive unit tests for @ref GenerationPipeline.
 *
 * Tests verify that the pipeline correctly orchestrates emitters, writes
 * files to disk, creates parent directories, and handles edge cases.
 */

#include "CPGen/Pipeline/GenerationPipeline.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Resolved.hpp"
#include "CPGen/Pipeline/Emitter.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// ============================================================================
// Helpers
// ============================================================================

namespace {

/// @brief Minimal emitter that returns a fixed set of output files.
class StubEmitter : public Emitter {
public:
  explicit StubEmitter(std::vector<OutputFile> files)
      : m_files(std::move(files)) {}

  [[nodiscard]] std::vector<OutputFile>
  emit(const ResolvedProject & /*project*/) override {
    return m_files;
  }

private:
  std::vector<OutputFile> m_files;
};

ResolvedProject makeMinimalProject() {
  ResolvedProject project;
  project.config.name = "PipeTest";
  project.config.path = "/tmp/";
  return project;
}

std::string readFileContent(const std::filesystem::path &path) {
  std::ifstream file(path);
  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

/// @brief RAII guard that removes a directory tree on destruction.
class TempDir {
public:
  explicit TempDir(const std::string &suffix) {
    m_path = std::filesystem::temp_directory_path() / ("cpgen_test_" + suffix);
    std::filesystem::remove_all(m_path); // clean slate
  }

  ~TempDir() { std::filesystem::remove_all(m_path); }

  TempDir(const TempDir &) = delete;
  TempDir &operator=(const TempDir &) = delete;
  TempDir(TempDir &&) = delete;
  TempDir &operator=(TempDir &&) = delete;

  [[nodiscard]] const std::filesystem::path &path() const { return m_path; }

private:
  std::filesystem::path m_path;
};

} // namespace

// ============================================================================
// Basic execution
// ============================================================================

TEST(UTGenerationPipeline, CreatesOutputDirectory) {
  TempDir dir("pipeline_mkdir");
  auto output = dir.path() / "NewProject";

  GenerationPipeline pipeline;
  pipeline.execute(makeMinimalProject(), output);

  EXPECT_TRUE(std::filesystem::exists(output));
  EXPECT_TRUE(std::filesystem::is_directory(output));
}

TEST(UTGenerationPipeline, WritesSingleFileFromEmitter) {
  TempDir dir("pipeline_single");
  auto output = dir.path() / "Proj";

  GenerationPipeline pipeline;
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(
          std::vector<OutputFile>{{"hello.txt", "world"}}));
  pipeline.execute(makeMinimalProject(), output);

  auto file_path = output / "hello.txt";
  ASSERT_TRUE(std::filesystem::exists(file_path));
  EXPECT_EQ(readFileContent(file_path), "world");
}

TEST(UTGenerationPipeline, WritesMultipleFilesFromOneEmitter) {
  TempDir dir("pipeline_multi");
  auto output = dir.path() / "Proj";

  GenerationPipeline pipeline;
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(
          std::vector<OutputFile>{
              {"a.txt", "alpha"},
              {"b.txt", "beta"}}));
  pipeline.execute(makeMinimalProject(), output);

  EXPECT_TRUE(std::filesystem::exists(output / "a.txt"));
  EXPECT_TRUE(std::filesystem::exists(output / "b.txt"));
  EXPECT_EQ(readFileContent(output / "a.txt"), "alpha");
  EXPECT_EQ(readFileContent(output / "b.txt"), "beta");
}

// ============================================================================
// Multiple emitters
// ============================================================================

TEST(UTGenerationPipeline, CombinesOutputFromMultipleEmitters) {
  TempDir dir("pipeline_combine");
  auto output = dir.path() / "Proj";

  GenerationPipeline pipeline;
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(
          std::vector<OutputFile>{{"file1.txt", "from_emitter_1"}}));
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(
          std::vector<OutputFile>{{"file2.txt", "from_emitter_2"}}));
  pipeline.execute(makeMinimalProject(), output);

  EXPECT_EQ(readFileContent(output / "file1.txt"), "from_emitter_1");
  EXPECT_EQ(readFileContent(output / "file2.txt"), "from_emitter_2");
}

// ============================================================================
// Nested directory creation
// ============================================================================

TEST(UTGenerationPipeline, CreatesParentDirectoriesForNestedFiles) {
  TempDir dir("pipeline_nested");
  auto output = dir.path() / "Proj";

  GenerationPipeline pipeline;
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(
          std::vector<OutputFile>{{"deep/nested/dir/file.txt", "nested!"}}));
  pipeline.execute(makeMinimalProject(), output);

  auto file_path = output / "deep" / "nested" / "dir" / "file.txt";
  ASSERT_TRUE(std::filesystem::exists(file_path));
  EXPECT_EQ(readFileContent(file_path), "nested!");
}

// ============================================================================
// No emitters — empty directory
// ============================================================================

TEST(UTGenerationPipeline, NoEmittersProducesEmptyDirectory) {
  TempDir dir("pipeline_empty");
  auto output = dir.path() / "Proj";

  GenerationPipeline pipeline;
  pipeline.execute(makeMinimalProject(), output);

  EXPECT_TRUE(std::filesystem::exists(output));
  EXPECT_TRUE(std::filesystem::is_empty(output));
}

// ============================================================================
// Overwrite existing file
// ============================================================================

TEST(UTGenerationPipeline, OverwritesExistingFile) {
  TempDir dir("pipeline_overwrite");
  auto output = dir.path() / "Proj";
  std::filesystem::create_directories(output);

  // Write initial content
  {
    std::ofstream f(output / "config.txt");
    f << "old content";
  }

  GenerationPipeline pipeline;
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(
          std::vector<OutputFile>{{"config.txt", "new content"}}));
  pipeline.execute(makeMinimalProject(), output);

  EXPECT_EQ(readFileContent(output / "config.txt"), "new content");
}

// ============================================================================
// Emitter returning empty vector
// ============================================================================

TEST(UTGenerationPipeline, EmitterReturningEmptyVectorIsHarmless) {
  TempDir dir("pipeline_empty_emitter");
  auto output = dir.path() / "Proj";

  GenerationPipeline pipeline;
  pipeline.addEmitter(
      std::make_unique<StubEmitter>(std::vector<OutputFile>{}));
  pipeline.execute(makeMinimalProject(), output);

  EXPECT_TRUE(std::filesystem::exists(output));
  EXPECT_TRUE(std::filesystem::is_empty(output));
}

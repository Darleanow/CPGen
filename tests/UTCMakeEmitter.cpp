/**
 * @file UTCMakeEmitter.cpp
 * @brief Exhaustive unit tests for @ref CMakeEmitter.
 *
 * Tests cover the full assembly pipeline as well as edge cases for individual
 * fragment generators (via the public @ref CMakeEmitter::emit interface).
 */

#include "CPGen/Pipeline/CMakeEmitter.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Module.hpp"
#include "CPGen/Core/Resolved.hpp"

#include <gtest/gtest.h>
#include <string>

// ============================================================================
// Helpers — build ResolvedProject fixtures
// ============================================================================

namespace {

ResolvedProject makeMinimalProject() {
  ResolvedProject project;
  project.config.name = "MyProject";
  project.config.path = "/tmp/";
  project.config.standard = CppStandard::Cpp20;
  return project;
}

ResolvedProject makeProjectWithGtest() {
  auto project = makeMinimalProject();
  project.config.modules = {"gtest"};

  ResolvedModule mod;
  mod.name = "gtest";
  mod.fetch_content.emplace();
  mod.fetch_content->module_name = "googletest";
  mod.fetch_content->url = "https://example.com/gtest.zip";
  project.modules.push_back(mod);

  TargetInjection inj;
  inj.target_name = "test";
  inj.module_name = "gtest";
  inj.link = {"GTest::gtest_main"};
  inj.cmake_include = "GoogleTest";
  inj.post_link = "gtest_discover_tests(${TARGET_NAME})";
  project.injections.push_back(inj);

  ResolvedTarget target;
  target.type = TargetType::Test;
  target.name = "test";
  target.path = "/tmp/MyProject/tests";
  project.targets.push_back(target);

  return project;
}

} // namespace

// ============================================================================
// emit() output structure
// ============================================================================

TEST(UTCMakeEmitter, EmitReturnsSingleFile) {
  CMakeEmitter emitter;
  auto files = emitter.emit(makeMinimalProject());
  ASSERT_EQ(files.size(), 1U);
}

TEST(UTCMakeEmitter, EmitFileNameIsCMakeLists) {
  CMakeEmitter emitter;
  auto files = emitter.emit(makeMinimalProject());
  EXPECT_EQ(files[0].relative_path, "CMakeLists.txt");
}

TEST(UTCMakeEmitter, EmitContentIsNonEmpty) {
  CMakeEmitter emitter;
  auto files = emitter.emit(makeMinimalProject());
  EXPECT_FALSE(files[0].content.empty());
}

// ============================================================================
// Header block
// ============================================================================

TEST(UTCMakeEmitter, ContainsCMakeMinimumRequired) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("cmake_minimum_required(VERSION"), std::string::npos);
}

TEST(UTCMakeEmitter, ContainsProjectName) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("MyProject"), std::string::npos);
}

TEST(UTCMakeEmitter, ContainsProjectDeclaration) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("project("), std::string::npos);
  EXPECT_NE(content.find("VERSION 1.0.0"), std::string::npos);
  EXPECT_NE(content.find("LANGUAGES CXX"), std::string::npos);
}

// ============================================================================
// C++ Standard
// ============================================================================

TEST(UTCMakeEmitter, StandardCpp17) {
  auto project = makeMinimalProject();
  project.config.standard = CppStandard::Cpp17;
  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("CMAKE_CXX_STANDARD 17"), std::string::npos);
}

TEST(UTCMakeEmitter, StandardCpp20) {
  auto project = makeMinimalProject();
  project.config.standard = CppStandard::Cpp20;
  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("CMAKE_CXX_STANDARD 20"), std::string::npos);
}

TEST(UTCMakeEmitter, StandardCpp23) {
  auto project = makeMinimalProject();
  project.config.standard = CppStandard::Cpp23;
  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("CMAKE_CXX_STANDARD 23"), std::string::npos);
}

TEST(UTCMakeEmitter, AlwaysSetsStandardRequired) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("CMAKE_CXX_STANDARD_REQUIRED ON"), std::string::npos);
}

TEST(UTCMakeEmitter, AlwaysSetsExtensionsOff) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("CMAKE_CXX_EXTENSIONS OFF"), std::string::npos);
}

TEST(UTCMakeEmitter, AlwaysExportsCompileCommands) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("CMAKE_EXPORT_COMPILE_COMMANDS ON"), std::string::npos);
}

// ============================================================================
// Compile options
// ============================================================================

TEST(UTCMakeEmitter, ContainsAddCompileOptions) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("add_compile_options("), std::string::npos);
}

TEST(UTCMakeEmitter, CompileOptionsIncludeWall) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("-Wall"), std::string::npos);
}

TEST(UTCMakeEmitter, CompileOptionsIncludeWerror) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("-Werror"), std::string::npos);
}

// ============================================================================
// FetchContent — no modules
// ============================================================================

TEST(UTCMakeEmitter, NoModulesNoFetchContent) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_EQ(content.find("include(FetchContent)"), std::string::npos);
  EXPECT_EQ(content.find("FetchContent_Declare"), std::string::npos);
  EXPECT_EQ(content.find("FetchContent_MakeAvailable"), std::string::npos);
}

// ============================================================================
// FetchContent — with modules
// ============================================================================

TEST(UTCMakeEmitter, WithModulesHasFetchContentInclude) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("include(FetchContent)"), std::string::npos);
}

TEST(UTCMakeEmitter, WithModulesHasFetchContentDeclare) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("FetchContent_Declare("), std::string::npos);
  EXPECT_NE(content.find("googletest"), std::string::npos);
  EXPECT_NE(content.find("https://example.com/gtest.zip"), std::string::npos);
}

TEST(UTCMakeEmitter, WithModulesHasMakeAvailable) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("FetchContent_MakeAvailable(googletest)"),
            std::string::npos);
}

// ============================================================================
// Clang-tidy
// ============================================================================

TEST(UTCMakeEmitter, NoClangTidyByDefault) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_EQ(content.find("CLANG_TIDY"), std::string::npos);
}

TEST(UTCMakeEmitter, ClangTidyEnabledContainsBlock) {
  auto project = makeMinimalProject();
  project.config.tooling.clang_tidy = true;
  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("find_program(CLANG_TIDY_EXE"), std::string::npos);
  EXPECT_NE(content.find("CMAKE_CXX_CLANG_TIDY"), std::string::npos);
  EXPECT_NE(content.find("header-filter"), std::string::npos);
}

// ============================================================================
// Main executable target
// ============================================================================

TEST(UTCMakeEmitter, AlwaysHasMainExecutable) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("add_executable(MyProject"), std::string::npos);
  EXPECT_NE(content.find("src/main.cpp"), std::string::npos);
}

TEST(UTCMakeEmitter, MainExecutableHasIncludeDirectories) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_NE(content.find("target_include_directories(MyProject"),
            std::string::npos);
  EXPECT_NE(content.find("CMAKE_CURRENT_SOURCE_DIR"), std::string::npos);
}

// ============================================================================
// Test target injection
// ============================================================================

TEST(UTCMakeEmitter, TestTargetDeclaration) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("add_executable(test_suite"), std::string::npos);
  EXPECT_NE(content.find("tests/test_main.cpp"), std::string::npos);
}

TEST(UTCMakeEmitter, TestTargetLinkLibraries) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("target_link_libraries(test_suite PUBLIC "
                          "GTest::gtest_main)"),
            std::string::npos);
}

TEST(UTCMakeEmitter, TestTargetCMakeInclude) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("include(GoogleTest)"), std::string::npos);
}

TEST(UTCMakeEmitter, TestTargetPostLinkWithSubstitution) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  // ${TARGET_NAME} should be replaced with "test_suite"
  EXPECT_NE(content.find("gtest_discover_tests(test_suite)"),
            std::string::npos);
  EXPECT_EQ(content.find("${TARGET_NAME}"), std::string::npos);
}

TEST(UTCMakeEmitter, TestTargetEnablesTesting) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeProjectWithGtest())[0].content;
  EXPECT_NE(content.find("enable_testing()"), std::string::npos);
}

TEST(UTCMakeEmitter, NoTestTargetNoEnableTesting) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_EQ(content.find("enable_testing()"), std::string::npos);
}

// ============================================================================
// Library target convention
// ============================================================================

TEST(UTCMakeEmitter, LibraryTargetUsesAddLibrary) {
  auto project = makeMinimalProject();
  ResolvedTarget target;
  target.type = TargetType::Library;
  target.name = "library";
  target.path = "/tmp/MyProject/src";
  project.targets.push_back(target);

  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("add_library(MyProjectLib"), std::string::npos);
  EXPECT_NE(content.find("src/lib.cpp"), std::string::npos);
}

// ============================================================================
// Multiple modules
// ============================================================================

TEST(UTCMakeEmitter, MultipleFetchContentModules) {
  auto project = makeMinimalProject();

  ResolvedModule mod1;
  mod1.name = "gtest";
  mod1.fetch_content.emplace();
  mod1.fetch_content->module_name = "googletest";
  mod1.fetch_content->url = "https://example.com/gtest.zip";

  ResolvedModule mod2;
  mod2.name = "spdlog";
  mod2.fetch_content.emplace();
  mod2.fetch_content->module_name = "spdlog";
  mod2.fetch_content->url = "https://example.com/spdlog.zip";

  project.modules = {mod1, mod2};

  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("googletest"), std::string::npos);
  EXPECT_NE(content.find("spdlog"), std::string::npos);
  EXPECT_NE(content.find("FetchContent_MakeAvailable(googletest spdlog)"),
            std::string::npos);
}

// ============================================================================
// Injection with no optional fields
// ============================================================================

TEST(UTCMakeEmitter, InjectionWithoutCMakeIncludeOrPostLink) {
  auto project = makeMinimalProject();

  ResolvedTarget target;
  target.type = TargetType::Test;
  target.name = "test";
  project.targets.push_back(target);

  TargetInjection inj;
  inj.target_name = "test";
  inj.module_name = "something";
  inj.link = {"SomeLib"};
  // cmake_include and post_link deliberately left as nullopt
  project.injections.push_back(inj);

  CMakeEmitter emitter;
  const auto content = emitter.emit(project)[0].content;
  EXPECT_NE(content.find("target_link_libraries(test_suite PUBLIC SomeLib)"),
            std::string::npos);
}

// ============================================================================
// Order validation — cmake_minimum_required comes first
// ============================================================================

TEST(UTCMakeEmitter, CMakeMinimumRequiredIsFirst) {
  CMakeEmitter emitter;
  const auto content = emitter.emit(makeMinimalProject())[0].content;
  EXPECT_EQ(content.find("cmake_minimum_required"), 0U);
}

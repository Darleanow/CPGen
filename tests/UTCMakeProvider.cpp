#include "CPGen/Providers/CMakeProvider.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Module.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>

TEST(UTCMakeProvider, MinimumRequiredContainsKeyword) {
  const auto result = CMakeProvider::getMinimumRequired(3.31F);
  EXPECT_NE(result.find("cmake_minimum_required"), std::string::npos);
}

TEST(UTCMakeProvider, MinimumRequiredContainsVersion) {
  const auto result = CMakeProvider::getMinimumRequired(3.31F);
  EXPECT_NE(result.find("3.31"), std::string::npos);
}

TEST(UTCMakeProvider, MinimumRequiredStripsTrailingZero) {
  const auto result = CMakeProvider::getMinimumRequired(3.0F);
  EXPECT_NE(result.find("VERSION 3)"), std::string::npos);
}

TEST(UTCMakeProvider, ProjectHeaderContainsProjectKeyword) {
  const auto result = CMakeProvider::getProjectHeader("MyProject");
  EXPECT_NE(result.find("project("), std::string::npos);
}

TEST(UTCMakeProvider, ProjectHeaderContainsName) {
  const auto result = CMakeProvider::getProjectHeader("MyProject");
  EXPECT_NE(result.find("MyProject"), std::string::npos);
}

TEST(UTCMakeProvider, ProjectHeaderContainsVersion) {
  const auto result = CMakeProvider::getProjectHeader("MyProject");
  EXPECT_NE(result.find("VERSION 1.0.0"), std::string::npos);
}

TEST(UTCMakeProvider, ProjectHeaderContainsCxxLanguage) {
  const auto result = CMakeProvider::getProjectHeader("MyProject");
  EXPECT_NE(result.find("LANGUAGES CXX"), std::string::npos);
}

TEST(UTCMakeProvider, StandardCpp17SetsCorrectValue) {
  const auto result = CMakeProvider::getStandard(CppStandard::Cpp17);
  EXPECT_NE(result.find("CMAKE_CXX_STANDARD 17"), std::string::npos);
}

TEST(UTCMakeProvider, StandardCpp20SetsCorrectValue) {
  const auto result = CMakeProvider::getStandard(CppStandard::Cpp20);
  EXPECT_NE(result.find("CMAKE_CXX_STANDARD 20"), std::string::npos);
}

TEST(UTCMakeProvider, StandardCpp23SetsCorrectValue) {
  const auto result = CMakeProvider::getStandard(CppStandard::Cpp23);
  EXPECT_NE(result.find("CMAKE_CXX_STANDARD 23"), std::string::npos);
}

TEST(UTCMakeProvider, StandardAlwaysEnablesRequired) {
  const auto result = CMakeProvider::getStandard(CppStandard::Cpp20);
  EXPECT_NE(result.find("CMAKE_CXX_STANDARD_REQUIRED ON"), std::string::npos);
}

TEST(UTCMakeProvider, IncludeFetchContentExactOutput) {
  EXPECT_EQ(CMakeProvider::getIncludeFetchContent(), "include(FetchContent)");
}

TEST(UTCMakeProvider, DeclareModuleContainsFetchContentDeclare) {
  const FetchContent fc{"googletest", "https://example.com/gtest.zip"};
  const auto result = CMakeProvider::getDeclareModule(fc);
  EXPECT_NE(result.find("FetchContent_Declare"), std::string::npos);
}

TEST(UTCMakeProvider, DeclareModuleContainsModuleName) {
  const FetchContent fc{"googletest", "https://example.com/gtest.zip"};
  const auto result = CMakeProvider::getDeclareModule(fc);
  EXPECT_NE(result.find("googletest"), std::string::npos);
}

TEST(UTCMakeProvider, DeclareModuleContainsUrl) {
  const FetchContent fc{"googletest", "https://example.com/gtest.zip"};
  const auto result = CMakeProvider::getDeclareModule(fc);
  EXPECT_NE(result.find("https://example.com/gtest.zip"), std::string::npos);
}

TEST(UTCMakeProvider, MakeAvailableContainsFetchContentMakeAvailable) {
  const auto result = CMakeProvider::getMakeAvailable({"googletest"});
  EXPECT_NE(result.find("FetchContent_MakeAvailable"), std::string::npos);
}

TEST(UTCMakeProvider, MakeAvailableSingleModule) {
  const auto result = CMakeProvider::getMakeAvailable({"googletest"});
  EXPECT_NE(result.find("googletest"), std::string::npos);
}

TEST(UTCMakeProvider, MakeAvailableMultipleModules) {
  const auto result = CMakeProvider::getMakeAvailable({"googletest", "spdlog"});
  EXPECT_NE(result.find("googletest"), std::string::npos);
  EXPECT_NE(result.find("spdlog"), std::string::npos);
}

TEST(UTCMakeProvider, MakeAvailableNoTrailingSpace) {
  const auto result = CMakeProvider::getMakeAvailable({"gtest"});
  EXPECT_EQ(result.back(), ')');
}

TEST(UTCMakeProvider, MakeAvailableEmptyThrows) {
  EXPECT_THROW(CMakeProvider::getMakeAvailable({}), std::invalid_argument);
}

TEST(UTCMakeProvider, CompileOptionsContainsAddCompileOptions) {
  const auto result = CMakeProvider::getCompileOptions();
  EXPECT_NE(result.find("add_compile_options"), std::string::npos);
}

TEST(UTCMakeProvider, CompileOptionsContainsWall) {
  EXPECT_NE(CMakeProvider::getCompileOptions().find("-Wall"), std::string::npos);
}

TEST(UTCMakeProvider, CompileOptionsContainsWerror) {
  EXPECT_NE(CMakeProvider::getCompileOptions().find("-Werror"), std::string::npos);
}

TEST(UTCMakeProvider, ClangTidyContainsFindProgram) {
  const auto result = CMakeProvider::getClangTidyTooling();
  EXPECT_NE(result.find("find_program(CLANG_TIDY_EXE"), std::string::npos);
}

TEST(UTCMakeProvider, ClangTidyContainsCmakeVariable) {
  const auto result = CMakeProvider::getClangTidyTooling();
  EXPECT_NE(result.find("CMAKE_CXX_CLANG_TIDY"), std::string::npos);
}

TEST(UTCMakeProvider, ClangTidyContainsHeaderFilter) {
  const auto result = CMakeProvider::getClangTidyTooling();
  EXPECT_NE(result.find("header-filter"), std::string::npos);
}

TEST(UTCMakeProvider, TargetDeclarationExecutableUsesAddExecutable) {
  const std::vector<std::filesystem::path> files{"/home/user/MyProject/src/main.cpp"};
  const auto result = CMakeProvider::getTargetDeclaration(
      "MyProject", TargetType::Executable, "myapp", files);
  EXPECT_NE(result.find("add_executable"), std::string::npos);
}

TEST(UTCMakeProvider, TargetDeclarationLibraryUsesAddLibrary) {
  const std::vector<std::filesystem::path> files{"/home/user/MyProject/src/lib.cpp"};
  const auto result = CMakeProvider::getTargetDeclaration(
      "MyProject", TargetType::Library, "mylib", files);
  EXPECT_NE(result.find("add_library"), std::string::npos);
}

TEST(UTCMakeProvider, TargetDeclarationTestUsesAddExecutable) {
  const std::vector<std::filesystem::path> files{"/home/user/MyProject/tests/test.cpp"};
  const auto result = CMakeProvider::getTargetDeclaration(
      "MyProject", TargetType::Test, "test_suite", files);
  EXPECT_NE(result.find("add_executable"), std::string::npos);
}

TEST(UTCMakeProvider, TargetDeclarationContainsTargetName) {
  const std::vector<std::filesystem::path> files{"/home/user/MyProject/src/main.cpp"};
  const auto result = CMakeProvider::getTargetDeclaration(
      "MyProject", TargetType::Executable, "myapp", files);
  EXPECT_NE(result.find("myapp"), std::string::npos);
}

TEST(UTCMakeProvider, TargetDeclarationPathIsRelativeToProject) {
  const std::vector<std::filesystem::path> files{"/home/user/MyProject/src/main.cpp"};
  const auto result = CMakeProvider::getTargetDeclaration(
      "MyProject", TargetType::Executable, "myapp", files);
  EXPECT_NE(result.find("MyProject/src/main.cpp"), std::string::npos);
  EXPECT_EQ(result.find("/home/user/"), std::string::npos);
}

TEST(UTCMakeProvider, TargetDeclarationEmptyFilesThrows) {
  EXPECT_THROW(
      CMakeProvider::getTargetDeclaration("MyProject", TargetType::Executable,
                                          "myapp", {}),
      std::invalid_argument);
}

TEST(UTCMakeProvider, IncludeDirectoriesContainsTargetName) {
  const auto result = CMakeProvider::getIncludeDirectories("myapp");
  EXPECT_NE(result.find("myapp"), std::string::npos);
}

TEST(UTCMakeProvider, IncludeDirectoriesContainsCmakeSourceDir) {
  const auto result = CMakeProvider::getIncludeDirectories("myapp");
  EXPECT_NE(result.find("CMAKE_CURRENT_SOURCE_DIR"), std::string::npos);
}

TEST(UTCMakeProvider, IncludeDirectoriesPointsToIncludeFolder) {
  const auto result = CMakeProvider::getIncludeDirectories("myapp");
  EXPECT_NE(result.find("/include)"), std::string::npos);
}

TEST(UTCMakeProvider, TargetLinkContainsTargetLinkLibraries) {
  const auto result = CMakeProvider::getTargetLink("myapp", {"SomeLib"});
  EXPECT_NE(result.find("target_link_libraries"), std::string::npos);
}

TEST(UTCMakeProvider, TargetLinkContainsTargetName) {
  const auto result = CMakeProvider::getTargetLink("myapp", {"SomeLib"});
  EXPECT_NE(result.find("myapp"), std::string::npos);
}

TEST(UTCMakeProvider, TargetLinkContainsSingleLib) {
  const auto result = CMakeProvider::getTargetLink("myapp", {"GTest::gtest_main"});
  EXPECT_NE(result.find("GTest::gtest_main"), std::string::npos);
}

TEST(UTCMakeProvider, TargetLinkContainsMultipleLibs) {
  const auto result =
      CMakeProvider::getTargetLink("myapp", {"GTest::gtest_main", "fmt::fmt"});
  EXPECT_NE(result.find("GTest::gtest_main"), std::string::npos);
  EXPECT_NE(result.find("fmt::fmt"), std::string::npos);
}

TEST(UTCMakeProvider, TargetLinkUsesPublicScope) {
  const auto result = CMakeProvider::getTargetLink("myapp", {"SomeLib"});
  EXPECT_NE(result.find("PUBLIC"), std::string::npos);
}

TEST(UTCMakeProvider, TargetLinkEmptyLibsThrows) {
  EXPECT_THROW(CMakeProvider::getTargetLink("myapp", {}), std::invalid_argument);
}

TEST(UTCMakeProvider, EnableTestsExactOutput) {
  EXPECT_EQ(CMakeProvider::getEnableTests(), "enable_testing()");
}

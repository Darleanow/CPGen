#include "CPGen/Providers/CMakeProvider.hpp"
#include "CPGen/Core/Config.hpp"
#include <format>
#include <stdexcept>

std::string CMakeProvider::toString(CppStandard standard) noexcept {
  switch (standard) {
  case CppStandard::Cpp17:
    return "17";
  case CppStandard::Cpp20:
    return "20";
  case CppStandard::Cpp23:
  default:
    return "23";
  }
}

std::string CMakeProvider::getMinimumRequired(float version) {
  return "cmake_minimum_required(VERSION " + std::format("{:g}", version) + ")";
}

std::string CMakeProvider::getProjectHeader(const std::string &name) {
  return "project(\n  " + name + "\n  VERSION 1.0.0\n LANGUAGES CXX)";
}

std::string CMakeProvider::getStandard(CppStandard standard) {
  return "set(CMAKE_CXX_STANDARD " + toString(standard) +
         ")\nset(CMAKE_CXX_STANDARD_REQUIRED ON)";
}

std::string CMakeProvider::getIncludeFetchContent() {
  return "include(FetchContent)";
}

std::string CMakeProvider::getDeclareModule(const FetchContent &fetch_content) {
  return "FetchContent_Declare(\n  " + fetch_content.module_name + "\n  URL " +
         fetch_content.url + "\n)";
}

std::string
CMakeProvider::getMakeAvailable(const std::vector<std::string> &modules) {
  if (modules.empty()) {
    throw std::invalid_argument(
        "getMakeAvailable: modules list must not be empty");
  }

  std::string mods;
  for (const auto &mod : modules) {
    mods += mod + " ";
  }
  mods.pop_back();

  return "FetchContent_MakeAvailable(\n  " + mods + "\n)";
}

std::string CMakeProvider::getCompileOptions() {
  return "add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion "
         "-Werror)";
}

std::string CMakeProvider::getClangTidyTooling() {
  return "find_program(CLANG_TIDY_EXE NAMES clang-tidy)"
         "\nif(CLANG_TIDY_EXE)"
         "\n  set(CMAKE_CXX_CLANG_TIDY"
         "\n    ${CLANG_TIDY_EXE}"
         "\n    \"--header-filter=${CMAKE_SOURCE_DIR}/(include|src)/.*\")"
         "\nendif()";
}

std::string CMakeProvider::getTargetDeclaration(
    const std::string &project_name, TargetType type,
    const std::string &target_name,
    const std::vector<std::filesystem::path> &files) {
  if (files.empty()) {
    throw std::invalid_argument(
        "getTargetDeclaration: files list must not be empty");
  }

  const std::string base_instruction =
      type == TargetType::Library ? "add_library" : "add_executable";

  std::string file_list;
  for (const auto &file : files) {
    auto filestr = file.string();
    const auto pos = filestr.find(project_name);
    file_list +=
        (pos != std::string::npos ? filestr.substr(pos) : filestr) + "\n  ";
  }
  file_list.resize(file_list.size() - 2);

  return base_instruction + "(\n  " + target_name + "\n  " + file_list + ")";
}

std::string
CMakeProvider::getIncludeDirectories(const std::string &target_name) {
  return "target_include_directories(" + target_name +
         "\n"
         "                          PUBLIC "
         "${CMAKE_CURRENT_SOURCE_DIR}/include)";
}

std::string CMakeProvider::getTargetLink(const std::string &target_name,
                                         const std::vector<std::string> &libs) {
  if (libs.empty()) {
    throw std::invalid_argument("getTargetLink: libs list must not be empty");
  }

  std::string libs_list;
  for (const auto &lib : libs) {
    libs_list += lib + " ";
  }
  libs_list.pop_back();

  // todo(Darleanow): Add way of configuring each pkg independently for scope
  // (PUBLIC, PRIVATE, INTERFACE).
  return "target_link_libraries( " + target_name + " PUBLIC " + libs_list + ")";
}

std::string CMakeProvider::getEnableTests() { return "enable_testing()"; }

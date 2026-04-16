/**
 * @file CMakeEmitter.cpp
 * @brief Implements the CMake build-file emitter.
 *
 * Contains both the individual fragment generators (absorbed from the former
 * @c CMakeProvider) and the @ref CMakeEmitter::assemble orchestrator that
 * chains them into a valid CMakeLists.txt.
 *
 * @author Darleanow
 */

#include "CPGen/Pipeline/CMakeEmitter.hpp"

#include <format>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

std::vector<OutputFile>
CMakeEmitter::emit(const ResolvedProject &project) {
  return {{"CMakeLists.txt", assemble(project)}};
}

// ---------------------------------------------------------------------------
// Assembler
// ---------------------------------------------------------------------------

std::string CMakeEmitter::assemble(const ResolvedProject &project) {
  std::string out;

  // Header ----------------------------------------------------------------
  out += minimumRequired(4.0F) + "\n";
  out += projectHeader(project.config.name) + "\n\n";
  out += cxxStandard(project.config.standard) + "\n";
  out += "set(CMAKE_CXX_EXTENSIONS OFF)\n";
  out += "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";

  // FetchContent -----------------------------------------------------------
  if (!project.modules.empty()) {
    out += fetchContentInclude() + "\n\n";

    std::vector<std::string> fc_names;
    for (const auto &mod : project.modules) {
      if (mod.fetch_content.has_value()) {
        out += declareModule(*mod.fetch_content) + "\n\n";
        fc_names.push_back(mod.fetch_content->module_name);
      }
    }
    if (!fc_names.empty()) {
      out += makeAvailable(fc_names) + "\n\n";
    }
  }

  // Compile options --------------------------------------------------------
  out += compileOptions() + "\n\n";

  // Clang-tidy -------------------------------------------------------------
  if (project.config.tooling.clang_tidy) {
    out += clangTidyBlock() + "\n\n";
  }

  // Main executable --------------------------------------------------------
  out += targetDeclaration(TargetType::Executable, project.config.name,
                           {"src/main.cpp"}) +
         "\n\n";
  out += includeDirectories(project.config.name) + "\n";

  // Injected targets -------------------------------------------------------
  bool has_tests = false;

  for (const auto &target : project.targets) {
    const auto tname = cmakeTargetName(target, project.config.name);
    const auto sources = conventionalSources(target);

    out += "\n";
    out += targetDeclaration(target.type, tname, sources) + "\n";
    out += applyInjections(project.injections, target.name, tname);

    if (target.type == TargetType::Test) {
      has_tests = true;
    }
  }

  // Testing ----------------------------------------------------------------
  if (has_tests) {
    out += "\n" + enableTesting() + "\n";
  }

  return out;
}

// ---------------------------------------------------------------------------
// Injection helper
// ---------------------------------------------------------------------------

std::string
CMakeEmitter::applyInjections(const std::vector<TargetInjection> &injections,
                              const std::string &target_name,       // NOLINT(bugprone-easily-swappable-parameters)
                              const std::string &cmake_name) {
  std::string out;

  for (const auto &inj : injections) {
    if (inj.target_name != target_name) {
      continue;
    }

    if (!inj.link.empty()) {
      out += targetLink(cmake_name, inj.link) + "\n";
    }
    if (inj.cmake_include.has_value()) {
      out += "\ninclude(" + *inj.cmake_include + ")\n";
    }
    if (inj.post_link.has_value()) {
      std::string post = *inj.post_link;
      const std::string placeholder = "${TARGET_NAME}";
      const auto pos = post.find(placeholder);
      if (pos != std::string::npos) {
        post.replace(pos, placeholder.size(), cmake_name);
      }
      out += post + "\n";
    }
  }

  return out;
}

// ---------------------------------------------------------------------------
// Target helpers
// ---------------------------------------------------------------------------

std::string CMakeEmitter::cmakeTargetName(const ResolvedTarget &target,
                                          const std::string &project_name) {
  switch (target.type) {
  case TargetType::Test:
    return "test_suite";
  case TargetType::Library:
    return project_name + "Lib";
  case TargetType::Executable:
  default:
    return project_name;
  }
}

std::vector<std::string>
CMakeEmitter::conventionalSources(const ResolvedTarget &target) {
  switch (target.type) {
  case TargetType::Test:
    return {"tests/test_main.cpp"};
  case TargetType::Library:
    return {"src/lib.cpp"};
  case TargetType::Executable:
  default:
    return {"src/main.cpp"};
  }
}

// ---------------------------------------------------------------------------
// Fragment generators (absorbed from CMakeProvider)
// ---------------------------------------------------------------------------

std::string CMakeEmitter::cppStandardString(CppStandard standard) noexcept {
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

std::string CMakeEmitter::minimumRequired(float version) {
  return "cmake_minimum_required(VERSION " + std::format("{:.1f}", version) +
         ")";
}

std::string CMakeEmitter::projectHeader(const std::string &name) {
  return "project(\n  " + name + "\n  VERSION 1.0.0\n  LANGUAGES CXX)\n";
}

std::string CMakeEmitter::cxxStandard(CppStandard standard) {
  return "set(CMAKE_CXX_STANDARD " + cppStandardString(standard) +
         ")\nset(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
}

std::string CMakeEmitter::fetchContentInclude() {
  return "include(FetchContent)";
}

std::string CMakeEmitter::declareModule(const FetchContent &fc) {
  return "FetchContent_Declare(\n  " + fc.module_name + "\n  URL " + fc.url +
         "\n)";
}

std::string
CMakeEmitter::makeAvailable(const std::vector<std::string> &modules) {
  if (modules.empty()) {
    throw std::invalid_argument(
        "makeAvailable: modules list must not be empty");
  }

  std::string mods;
  for (const auto &mod : modules) {
    if (!mods.empty()) {
      mods += ' ';
    }
    mods += mod;
  }

  return "FetchContent_MakeAvailable(" + mods + ")";
}

std::string CMakeEmitter::compileOptions() {
  return "add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion "
         "-Werror)";
}

std::string CMakeEmitter::clangTidyBlock() {
  return "find_program(CLANG_TIDY_EXE NAMES clang-tidy)"
         "\nif(CLANG_TIDY_EXE)"
         "\n  set(CMAKE_CXX_CLANG_TIDY"
         "\n    ${CLANG_TIDY_EXE}"
         "\n    \"--header-filter=${CMAKE_SOURCE_DIR}/(include|src)/.*\")"
         "\nendif()";
}

std::string
CMakeEmitter::targetDeclaration(TargetType type,
                                const std::string &target_name,
                                const std::vector<std::string> &sources) {
  if (sources.empty()) {
    throw std::invalid_argument(
        "targetDeclaration: sources list must not be empty");
  }

  const std::string cmd =
      type == TargetType::Library ? "add_library" : "add_executable";

  std::string file_list;
  for (const auto &src : sources) {
    file_list += "\n  " + src;
  }

  return cmd + "(" + target_name + file_list + "\n)";
}

std::string
CMakeEmitter::includeDirectories(const std::string &target_name) {
  return "target_include_directories(" + target_name +
         "\n                            PUBLIC "
         "${CMAKE_CURRENT_SOURCE_DIR}/include)";
}

std::string
CMakeEmitter::targetLink(const std::string &target_name,
                         const std::vector<std::string> &libs) {
  if (libs.empty()) {
    throw std::invalid_argument("targetLink: libs list must not be empty");
  }

  std::string libs_list;
  for (const auto &lib : libs) {
    if (!libs_list.empty()) {
      libs_list += ' ';
    }
    libs_list += lib;
  }

  return "target_link_libraries(" + target_name + " PUBLIC " + libs_list + ")";
}

std::string CMakeEmitter::enableTesting() { return "enable_testing()"; }

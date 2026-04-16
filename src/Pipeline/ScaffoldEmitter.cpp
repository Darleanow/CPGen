/**
 * @file ScaffoldEmitter.cpp
 * @brief Implements the project scaffold emitter.
 *
 * Generates the starter source files, README, .gitignore, and test skeleton
 * that form the initial directory tree of a new C++ project.  When a module
 * ships an example source file (e.g. @c templates/modules/gtest/example.cpp),
 * the emitter uses it instead of the built-in fallback.
 *
 * @author Darleanow
 */

#include "CPGen/Pipeline/ScaffoldEmitter.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

ScaffoldEmitter::ScaffoldEmitter(std::filesystem::path template_root)
    : m_template_root(std::move(template_root)) {}

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

std::vector<OutputFile>
ScaffoldEmitter::emit(const ResolvedProject &project) {
  std::vector<OutputFile> files;

  files.push_back({"src/main.cpp", generateMainCpp(project.config.name)});
  files.push_back({"README.md", generateReadme(project.config.name)});

  if (project.config.tooling.has_git) {
    files.push_back({".gitignore", generateGitignore()});
  }

  const bool has_test =
      std::any_of(project.targets.begin(), project.targets.end(),
                  [](const ResolvedTarget &t) {
                    return t.type == TargetType::Test;
                  });

  if (has_test) {
    files.push_back({"tests/test_main.cpp", generateTestMain(project)});
  }

  return files;
}

// ---------------------------------------------------------------------------
// Generators
// ---------------------------------------------------------------------------

std::string ScaffoldEmitter::generateMainCpp(const std::string &project_name) {
  return "#include <iostream>\n"
         "\n"
         "int main() {\n"
         "  std::cout << \"Hello, " +
         project_name +
         "!\\n\";\n"
         "\n"
         "  return 0;\n"
         "}\n";
}

std::string ScaffoldEmitter::generateGitignore() {
  return "build/\n"
         ".cache/\n"
         "compile_commands.json\n"
         "*.o\n"
         "*.a\n"
         "*.so\n"
         "*.dylib\n"
         "CMakeUserPresets.json\n";
}

std::string ScaffoldEmitter::generateReadme(const std::string &project_name) {
  return "# " + project_name +
         "\n\n"
         "Generated with [CPGen](https://github.com/Darleanow/CPGen).\n";
}

std::string
ScaffoldEmitter::generateTestMain(const ResolvedProject &project) {
  // Try to load an example file from the first test-injecting module.
  for (const auto &inj : project.injections) {
    if (inj.target_name != "test") {
      continue;
    }
    const auto example_path = m_template_root / "modules" /
                              inj.module_name / "example.cpp";
    if (std::filesystem::exists(example_path)) {
      std::ifstream file(example_path);
      if (file.is_open()) {
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
      }
    }
  }

  // Fallback: generic test skeleton.
  return "#include <gtest/gtest.h>\n"
         "\n"
         "TEST(SampleTest, BasicAssertion) {\n"
         "  EXPECT_EQ(1, 1);\n"
         "}\n";
}

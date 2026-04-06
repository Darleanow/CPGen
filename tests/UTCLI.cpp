#include "CPGen/CLI/CLI.hpp"
#include "CPGen/Core/Config.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace {

/**
 * @brief Owns a set of mutable argument strings and exposes argc/argv for CLI::parse.
 *
 * Avoids const_cast by storing each argument as a std::string and handing out
 * char* pointers via std::string::data() (non-const since C++17).
 */
class Args {
public:
  explicit Args(std::initializer_list<const char *> list) {
    for (const char *arg : list) {
      m_strings.emplace_back(arg);
    }
    refresh();
  }

  void push(const char *s) { m_strings.emplace_back(s); refresh(); }
  void pop()               { m_strings.pop_back();       refresh(); }

  int    argc()       { return static_cast<int>(m_ptrs.size()); }
  char **argv()       { return m_ptrs.data(); }

private:
  void refresh() {
    m_ptrs.clear();
    for (auto &s : m_strings) {
      m_ptrs.push_back(s.data());
    }
  }

  std::vector<std::string> m_strings;
  std::vector<char *>      m_ptrs;
};

} // namespace

TEST(UTCLI, PathValidationTests) {
  CLI cli;

  Args args{"-p"};
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("--path");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.push("-/path");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("/home");
  EXPECT_NO_THROW(cli.parse(args.argc(), args.argv()));

  args.pop(); args.push("/non_existing_weird/path123");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);
}

TEST(UTCLI, PathValueTests) {
  CLI cli;

  {
    Args args{"--path", "/home"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).path, "/home");
  }

  {
    Args args{"-p", "/tmp"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).path, "/tmp");
  }
}

TEST(UTCLI, NameValidationTests) {
  CLI cli;

  Args args{"-n"};
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("--name");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.push("-");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("TestValid_Name");
  EXPECT_NO_THROW(cli.parse(args.argc(), args.argv()));

  args.pop(); args.push("InvalidName-");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);
}

TEST(UTCLI, NameValueTests) {
  CLI cli;

  {
    Args args{"--name", "MyProject"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).name, "MyProject");
  }

  {
    Args args{"-n", "Project_123"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).name, "Project_123");
  }
}

TEST(UTCLI, TuiModeTests) {
  CLI cli;

  {
    Args args{"-u"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
  }

  {
    Args args{"--tui"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
  }
}

TEST(UTCLI, GitFlagTests) {
  CLI cli;

  {
    Args args{"-g"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.has_git);
  }

  {
    Args args{"--git"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.has_git);
  }

  {
    Args args{};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_FALSE(std::get<ProjectConfig>(result).tooling.has_git);
  }
}

TEST(UTCLI, StandardFlagTests) {
  CLI cli;

  Args args{"-s"};
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("--std");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.push("99");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("20");
  EXPECT_NO_THROW(cli.parse(args.argc(), args.argv()));

  auto result = cli.parse(args.argc(), args.argv());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  EXPECT_EQ(std::get<ProjectConfig>(result).standard, CppStandard::Cpp20);
}

TEST(UTCLI, ModulesFlagTests) {
  CLI cli;

  Args args{"-m"};
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("--modules");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.push("gtest");
  EXPECT_NO_THROW(cli.parse(args.argc(), args.argv()));

  {
    Args single{"-m", "gtest"};
    auto result = cli.parse(single.argc(), single.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    const auto &config = std::get<ProjectConfig>(result);
    ASSERT_EQ(config.modules.size(), 1U);
    EXPECT_EQ(config.modules[0], "gtest");
  }

  {
    Args multi{"-m", "gtest,spdlog"};
    auto result = cli.parse(multi.argc(), multi.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    const auto &config = std::get<ProjectConfig>(result);
    ASSERT_EQ(config.modules.size(), 2U);
    EXPECT_EQ(config.modules[0], "gtest");
    EXPECT_EQ(config.modules[1], "spdlog");
  }
}

TEST(UTCLI, TargetsFlagTests) {
  CLI cli;

  Args args{"-tar"};
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("--targets");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  {
    Args t{"-tar", "test"};
    auto result = cli.parse(t.argc(), t.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    ASSERT_EQ(std::get<ProjectConfig>(result).targets.size(), 1U);
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Test);
  }

  {
    Args t{"-tar", "exe"};
    auto result = cli.parse(t.argc(), t.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Executable);
  }

  {
    Args t{"--targets", "executable"};
    auto result = cli.parse(t.argc(), t.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Executable);
  }

  {
    Args t{"-tar", "lib"};
    auto result = cli.parse(t.argc(), t.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Library);
  }

  {
    Args t{"-tar", "exe,test"};
    auto result = cli.parse(t.argc(), t.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    const auto &config = std::get<ProjectConfig>(result);
    ASSERT_EQ(config.targets.size(), 2U);
    EXPECT_EQ(config.targets[0], TargetType::Executable);
    EXPECT_EQ(config.targets[1], TargetType::Test);
  }
}

TEST(UTCLI, ClangFormatFlagTests) {
  CLI cli;

  {
    Args args{"-cf"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_format);
  }

  {
    Args args{"--clang-format"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_format);
  }

  {
    Args args{};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_FALSE(std::get<ProjectConfig>(result).tooling.clang_format);
  }
}

TEST(UTCLI, ClangFormatPresetTests) {
  CLI cli;

  Args args{"-cfp"};
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.pop(); args.push("--cf-preset");
  EXPECT_THROW(cli.parse(args.argc(), args.argv()), std::runtime_error);

  args.push("Google");
  EXPECT_NO_THROW(cli.parse(args.argc(), args.argv()));

  auto result = cli.parse(args.argc(), args.argv());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  const auto &config = std::get<ProjectConfig>(result);
  ASSERT_TRUE(config.tooling.clang_format_preset.has_value());
  if (config.tooling.clang_format_preset.has_value()) {
    EXPECT_EQ(*config.tooling.clang_format_preset, "Google");
  }
}

TEST(UTCLI, ClangTidyFlagTests) {
  CLI cli;

  {
    Args args{"-ct"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_tidy);
  }

  {
    Args args{"--clang-tidy"};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_tidy);
  }

  {
    Args args{};
    auto result = cli.parse(args.argc(), args.argv());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_FALSE(std::get<ProjectConfig>(result).tooling.clang_tidy);
  }
}

TEST(UTCLI, EmptyArgsTest) {
  CLI cli;
  Args args{};
  auto result = cli.parse(args.argc(), args.argv());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  const auto &config = std::get<ProjectConfig>(result);
  EXPECT_TRUE(config.name.empty());
  EXPECT_TRUE(config.path.empty());
  EXPECT_FALSE(config.tooling.has_git);
  EXPECT_FALSE(config.tooling.clang_format);
  EXPECT_FALSE(config.tooling.clang_tidy);
  EXPECT_FALSE(config.tooling.clang_format_preset.has_value());
  EXPECT_TRUE(config.modules.empty());
  EXPECT_TRUE(config.targets.empty());
}

TEST(UTCLI, CombinedFlagsTests) {
  CLI cli;
  Args args{"-n", "MyProject", "-p", "/tmp", "-g", "-cf", "-ct"};

  auto result = cli.parse(args.argc(), args.argv());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  const auto &config = std::get<ProjectConfig>(result);
  EXPECT_EQ(config.name, "MyProject");
  EXPECT_EQ(config.path, "/tmp");
  EXPECT_TRUE(config.tooling.has_git);
  EXPECT_TRUE(config.tooling.clang_format);
  EXPECT_TRUE(config.tooling.clang_tidy);
}

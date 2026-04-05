#include "CPGen/CLI/CLI.hpp"
#include "CPGen/Core/Config.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <variant>

TEST(UTCLI, PathValidationTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-p"); // No arg for path
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--path"); // No arg for path
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"-/path"); // Ill formatted path
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"/home"); // Valid path
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  arr.pop_back();
  arr.push_back((char *)"/non_existing_weird/path123");
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);
}

TEST(UTCLI, PathValueTests) {
  CLI cli;

  {
    std::vector<char *> arr = {(char *)"--path", (char *)"/home"};
    auto result = cli.parse(2, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).path, "/home");
  }

  {
    std::vector<char *> arr = {(char *)"-p", (char *)"/tmp"};
    auto result = cli.parse(2, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).path, "/tmp");
  }
}

TEST(UTCLI, NameValidationTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-n"); // No arg for name
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--name"); // No arg for name
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"-"); // Invalid arg for name
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"TestValid_Name");
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  arr.pop_back();
  arr.push_back((char *)"InvalidName-");
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);
}

TEST(UTCLI, NameValueTests) {
  CLI cli;

  {
    std::vector<char *> arr = {(char *)"--name", (char *)"MyProject"};
    auto result = cli.parse(2, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).name, "MyProject");
  }

  {
    std::vector<char *> arr = {(char *)"-n", (char *)"Project_123"};
    auto result = cli.parse(2, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).name, "Project_123");
  }
}

TEST(UTCLI, TuiModeTests) {
  CLI cli;

  {
    std::vector<char *> arr = {(char *)"-u"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
  }

  {
    std::vector<char *> arr = {(char *)"--tui"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
  }
}

TEST(UTCLI, GitFlagTests) {
  CLI cli;

  {
    std::vector<char *> arr = {(char *)"-g"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.has_git);
  }

  {
    std::vector<char *> arr = {(char *)"--git"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.has_git);
  }

  {
    std::vector<char *> arr;
    auto result = cli.parse(0, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_FALSE(std::get<ProjectConfig>(result).tooling.has_git);
  }
}

TEST(UTCLI, StandardFlagTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-s"); // No arg for standard
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--std"); // No arg for standard
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"99"); // Out of range
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"20"); // Valid standard
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  auto result = cli.parse(2, arr.data());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  EXPECT_EQ(std::get<ProjectConfig>(result).standard, CppStandard::Cpp20);
}

TEST(UTCLI, ModulesFlagTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-m"); // No arg for modules
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--modules"); // No arg for modules
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"gtest"); // Single module
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  {
    std::vector<char *> single = {(char *)"-m", (char *)"gtest"};
    auto result = cli.parse(2, single.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    const auto &config = std::get<ProjectConfig>(result);
    ASSERT_EQ(config.modules.size(), 1U);
    EXPECT_EQ(config.modules[0], "gtest");
  }

  {
    std::vector<char *> multi = {(char *)"-m", (char *)"gtest,spdlog"};
    auto result = cli.parse(2, multi.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    const auto &config = std::get<ProjectConfig>(result);
    ASSERT_EQ(config.modules.size(), 2U);
    EXPECT_EQ(config.modules[0], "gtest");
    EXPECT_EQ(config.modules[1], "spdlog");
  }
}

TEST(UTCLI, TargetsFlagTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-tar"); // No arg for targets
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--targets"); // No arg for targets
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  {
    std::vector<char *> t = {(char *)"-tar", (char *)"test"};
    auto result = cli.parse(2, t.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    ASSERT_EQ(std::get<ProjectConfig>(result).targets.size(), 1U);
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Test);
  }

  {
    std::vector<char *> t = {(char *)"-tar", (char *)"exe"};
    auto result = cli.parse(2, t.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Executable);
  }

  {
    std::vector<char *> t = {(char *)"--targets", (char *)"executable"};
    auto result = cli.parse(2, t.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Executable);
  }

  {
    std::vector<char *> t = {(char *)"-tar", (char *)"lib"};
    auto result = cli.parse(2, t.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_EQ(std::get<ProjectConfig>(result).targets[0], TargetType::Library);
  }

  {
    std::vector<char *> t = {(char *)"-tar", (char *)"exe,test"};
    auto result = cli.parse(2, t.data());
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
    std::vector<char *> arr = {(char *)"-cf"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_format);
  }

  {
    std::vector<char *> arr = {(char *)"--clang-format"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_format);
  }

  {
    std::vector<char *> arr;
    auto result = cli.parse(0, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_FALSE(std::get<ProjectConfig>(result).tooling.clang_format);
  }
}

TEST(UTCLI, ClangFormatPresetTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-cfp"); // No arg for preset
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--cf-preset"); // No arg for preset
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"Google"); // Valid preset
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  auto result = cli.parse(2, arr.data());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  const auto &config = std::get<ProjectConfig>(result);
  ASSERT_TRUE(config.tooling.clang_format_preset.has_value());
  EXPECT_EQ(config.tooling.clang_format_preset.value(), "Google");
}

TEST(UTCLI, ClangTidyFlagTests) {
  CLI cli;

  {
    std::vector<char *> arr = {(char *)"-ct"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_tidy);
  }

  {
    std::vector<char *> arr = {(char *)"--clang-tidy"};
    auto result = cli.parse(1, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_TRUE(std::get<ProjectConfig>(result).tooling.clang_tidy);
  }

  {
    std::vector<char *> arr;
    auto result = cli.parse(0, arr.data());
    ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
    EXPECT_FALSE(std::get<ProjectConfig>(result).tooling.clang_tidy);
  }
}

TEST(UTCLI, EmptyArgsTest) {
  CLI cli;
  std::vector<char *> arr;
  auto result = cli.parse(0, arr.data());
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
  std::vector<char *> arr = {
      (char *)"-n",  (char *)"MyProject",
      (char *)"-p",  (char *)"/tmp",
      (char *)"-g",
      (char *)"-cf",
      (char *)"-ct",
  };

  auto result = cli.parse(7, arr.data());
  ASSERT_TRUE(std::holds_alternative<ProjectConfig>(result));
  const auto &config = std::get<ProjectConfig>(result);
  EXPECT_EQ(config.name, "MyProject");
  EXPECT_EQ(config.path, "/tmp");
  EXPECT_TRUE(config.tooling.has_git);
  EXPECT_TRUE(config.tooling.clang_format);
  EXPECT_TRUE(config.tooling.clang_tidy);
}

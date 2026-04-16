#include "CPGen/CLI/CLI.hpp"
#include "CPGen/Core/Config.hpp"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

std::string trim(std::string_view text) {
  const auto *first = std::find_if_not(text.begin(), text.end(), [](unsigned char c) {
    return std::isspace(c);
  });
  const auto *last = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char c) {
    return std::isspace(c);
  }).base();
  return std::string{first, last};
}

std::vector<std::string> splitCommaSeparated(std::string_view value) {
  std::vector<std::string> tokens;
  std::stringstream ss{std::string(value)};
  std::string token;
  while (std::getline(ss, token, ',')) {
    const auto trimmed = trim(token);
    if (trimmed.empty()) {
      throw std::runtime_error("Malformed comma-separated list: " + std::string(value));
    }
    tokens.push_back(trimmed);
  }
  return tokens;
}

TargetType parseTargetType(std::string_view value) {
  std::string normalized = trim(value);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });

  if (normalized == "exe" || normalized == "executable") {
    return TargetType::Executable;
  }
  if (normalized == "test") {
    return TargetType::Test;
  }
  return TargetType::Library;
}

} // namespace

Config CLI::parse(int argc, char **argv) {
  m_config = {};

  for (int i = 0; i < argc; i++) {
    const std::string flag = argv[i];
    if (flag == "-h" || flag == "--help") {
      showHelp();
      std::exit(0); // NOLINT
    }
    if (flag == "-u" || flag == "--tui") {
      return true;
    }
    parseProjectFlag(argc, argv, i);
    parseToolingFlag(argc, argv, i);
  }

  return m_config;
}

void CLI::parseName(int argc, char **argv, int &i) {
  ++i;
  if (i >= argc || argv[i][0] == '-') {
    throw std::runtime_error("No name provided!");
  }
  const std::regex valid_name("^\\w+$");
  std::cmatch match;
  if (!std::regex_search(argv[i], match, valid_name)) {
    throw std::runtime_error("Invalid name: must match ^\\w+$");
  }
  m_config.name = argv[i];
}

void CLI::parsePath(int argc, char **argv, int &i) {
  ++i;
  if (i >= argc || argv[i][0] == '-') {
    throw std::runtime_error("Path is ill formatted, aborting.");
  }
  if (!std::filesystem::exists(argv[i])) {
    throw std::runtime_error("Path does not exist on disk!");
  }
  m_config.path = argv[i];
}

void CLI::parseStandard(int argc, char **argv, int &i) {
  if (++i >= argc) {
    throw std::runtime_error("No standard provided!");
  }
  if (strcmp(argv[i], "17") == 0) {
    m_config.standard = CppStandard::Cpp17;
  } else if (strcmp(argv[i], "20") == 0) {
    m_config.standard = CppStandard::Cpp20;
  } else if (strcmp(argv[i], "23") == 0) {
    m_config.standard = CppStandard::Cpp23;
  } else {
    throw std::range_error("Standard not in range (17, 20, 23)!");
  }
}

void CLI::parseModules(int argc, char **argv, int &i) {
  if (++i >= argc) {
    throw std::runtime_error("No modules provided.");
  }

  const auto module_tokens = splitCommaSeparated(argv[i]);
  for (const auto &module_name : module_tokens) {
    if (module_name.empty()) {
      throw std::runtime_error("Empty module name in list: " + std::string(argv[i]));
    }
    m_config.modules.push_back(module_name);
  }
}

void CLI::parseTargets(int argc, char **argv, int &i) {
  if (++i >= argc) {
    throw std::runtime_error("No targets provided.");
  }

  const auto target_tokens = splitCommaSeparated(argv[i]);
  for (const auto &entry : target_tokens) {
    m_config.targets.push_back(parseTargetType(entry));
  }
}

void CLI::parseClangFormatPreset(int argc, char **argv, int &i) {
  if (++i >= argc) {
    throw std::runtime_error("No preset provided!");
  }
  m_config.tooling.clang_format_preset = argv[i];
}

void CLI::parseProjectFlag(int argc, char **argv, int &i) {
  const std::string flag = argv[i];
  if (flag == "-n" || flag == "--name") {
    parseName(argc, argv, i);
    return;
  }
  if (flag == "-p" || flag == "--path") {
    parsePath(argc, argv, i);
    return;
  }
  if (flag == "-s" || flag == "--std") {
    parseStandard(argc, argv, i);
    return;
  }
  if (flag == "-m" || flag == "--modules") {
    parseModules(argc, argv, i);
    return;
  }
  if (flag == "-tar" || flag == "--targets") {
    parseTargets(argc, argv, i);
  }
}

void CLI::parseToolingFlag(int argc, char **argv, int &i) {
  const std::string flag = argv[i];
  if (flag == "-g" || flag == "--git") {
    m_config.tooling.has_git = true;
    return;
  }
  if (flag == "-cf" || flag == "--clang-format") {
    m_config.tooling.clang_format = true;
    return;
  }
  if (flag == "-cfp" || flag == "--cf-preset") {
    parseClangFormatPreset(argc, argv, i);
    return;
  }
  if (flag == "-ct" || flag == "--clang-tidy") {
    m_config.tooling.clang_tidy = true;
  }
}

void CLI::showHelp() {
  std::cout
      << "CPGen - Command line options\n"
      << "-h    --help         | Shows this help.\n"
      << "-u    --tui          | Enable TUI mode.\n"
      << "-n    --name         | Sets the name of the project.\n"
      << "-p    --path         | Sets the target path of the project.\n"
      << "-g    --git          | Enable git.\n"
      << "-s    --std          | Set the standard {17, 20, 23}.\n"
      << "-m    --modules      | Modules, comma separated: -m gtest,spdlog\n"
      << "-tar  --targets      | Targets, comma separated: -tar exe,test\n"
      << "-cf   --clang-format | Enables clang-format.\n"
      << "-cfp  --cf-preset    | Clang-format preset to use.\n";
}

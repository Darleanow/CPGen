#include "CPGen/CLI/CLI.hpp"
#include "CPGen/Core/Config.hpp"
#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>
#include <stdexcept>

Config CLI::parse(const int argc, char **argv) {
  ProjectConfig config;

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      showHelp();
      exit(0);
    }

    if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--tui") == 0) {
      return true; // Early return, the options will be set via tui
    }

    if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
      i++;

      if (i >= argc || argv[i][0] == '-') {
        throw std::runtime_error("No name provided!");
      }

      std::regex valid_name("^\\w+$");
      std::cmatch match;
      if (!std::regex_search(argv[i], match, valid_name)) {
        throw std::runtime_error("Invalid name, must match ^\\w+$ (Upper, "
                                 "Lower, any or no digits).");
      }

      config.name = argv[i];
    }

    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) {
      i++;

      if (i >= argc || argv[i][0] == '-') {
        throw std::runtime_error("Path is ill formatted, aborting.");
      }

      if (!std::filesystem::exists(argv[i])) {
        throw std::runtime_error("Path does not exists on disk!");
      }

      config.path = argv[i];
    }

    if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--git") == 0) {
      config.tooling.has_git = true;
    }

    if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--std") == 0) {
      i++;
      if (i >= argc) {
        throw std::runtime_error("No standard provided!");
      }

      std::regex valid_standard("/17|20|23/gm");
      std::cmatch match;
      if (!std::regex_search(argv[i], match, valid_standard)) {
        throw std::range_error("Standard provided not in range (17,20,23)!");
      }

      if (strcmp(argv[i], "17") == 0) {
        config.standard = CppStandard::Cpp17;
      } else if (strcmp(argv[i], "20") == 0) {
        config.standard = CppStandard::Cpp20;
      } else {
        config.standard = CppStandard::Cpp23;
      }
    }

    if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--modules") == 0) {
      // parse modules: format module --> gtest, spdlog
      i++;
      if (i >= argc) {
        throw std::runtime_error("No modules provided.");
      }

      std::string curr;
      std::string modules_str = argv[i];
      std::stringstream ss(modules_str);
      std::string module;

      while (std::getline(ss, module, ',')) {
        config.modules.push_back(module);
      }
    }

    if (strcmp(argv[i], "-tar") == 0 || strcmp(argv[i], "--targets") == 0) {
      i++;
      if (i >= argc) {
        throw std::runtime_error("No target provided.");
      }

      std::string curr;
      std::string targets_str = argv[i];
      std::stringstream ss(targets_str);
      std::string target;

      while (std::getline(ss, target, ',')) {
        if (target == "test") {
          config.targets.push_back(TargetType::Test);
        } else if (target == "exe" || target == "executable") {
          config.targets.push_back(TargetType::Executable);
        } else {
          config.targets.push_back(TargetType::Library);
        }
      }
    }

    if (strcmp(argv[i], "-cf") == 0 || strcmp(argv[i], "--clang-format") == 0) {
      config.tooling.clang_format = true;
    }

    if (strcmp(argv[i], "-cfp") == 0 || strcmp(argv[i], "--cf-preset") == 0) {
      i++;
      if (i >= argc) {
        throw std::runtime_error("No preset provided!");
      }

      config.tooling.clang_format_preset = argv[i];
    }

    if (strcmp(argv[i], "-ct") == 0 || strcmp(argv[i], "--clang-tidy") == 0) {
      config.tooling.clang_tidy = true;
    }
  }

  return config;
}

void CLI::showHelp() const {
  std::cout
      << "CPGen - Command line options\n"
      << "-h    --help         | Shows this help.\n"
      << "-u    --tui          | Enable TUI mode.\n"
      << "-n    --name         | Sets the name of the project.\n"
      << "-p    --path         | Sets the target path of the project.\n"
      << "-g    --git          | Enable git ?\n"
      << "-s    --std          | Set the standard {17,20,23}.\n"
      << "-m    --modules      | Add modules, comma separated `-m gtest, "
         "spdlog`.\n"
      << "-tar  --targets      | Add targets, comma separated `-tar src, "
         "exe`.\n"
      << "-cf   --clang-format | Enables clang format.\n"
      << "-cfp  --cf-preset    | Specifiy the clang format preset to use.\n";
}

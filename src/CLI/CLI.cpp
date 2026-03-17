#include "CPGen/CLI/CLI.hpp"
#include <cstring>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <unordered_map>

const CLIOpts CLI::parse(const int argc, char **argv) {
  CLIOpts opts;

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--tui") == 0) {
      opts.is_tui_mode = true;
      return opts; // Early return, the options will be set via tui
    }

    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) {
      i++;

      if (i >= argc || argv[i][0] == '-') {
        throw std::runtime_error("Path is ill formatted, aborting.");
      }

      if (!std::filesystem::exists(argv[i])) {
        throw std::runtime_error("Path does not exists on disk!");
      }

      opts.path = argv[i];
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

      opts.name = argv[i];
    }

    if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--git") == 0) {
      opts.has_git = true;
    }

    if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--template") == 0) {
      i++;

      if (i >= argc) {
        throw std::runtime_error("No template argument provided!");
      }

      opts.use_template = true;
      opts.template_name = argv[i];

      return opts; // We use the template as args, early return.
    }
  }

  return opts;
}

std::unordered_map<std::string, std::string>
CLI::parse_opts(const CLIOpts &opts) {
  std::unordered_map<std::string, std::string> mapped_args = {
      {"PROJECT_NAME", opts.name},
      {"CXX_STANDARD", "20"},
  };

  return mapped_args;
}

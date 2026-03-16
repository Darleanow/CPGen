#include "CPGen/CLI/CLI.hpp"
#include "CPGen/CLI/Errno.hpp"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>
#include <unordered_map>

const CLIOpts &CLI::parse(const int argc, char **argv) {
  const auto param_error = [](const std::string &param_name) {
    std::cout << param_name << " argument is ill-formated.\n";
  };

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tui") == 0) {
      m_opts.is_tui_mode = true;
      return m_opts; // Early return, the options will be set via tui
    }

    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) {
      i++;

      if (i >= argc || argv[i][0] == '-') {
        param_error("Path");
        exit(ENOARG);
      }

      if (!std::filesystem::exists(argv[i])) {
        std::cout << "Path does not exists.\n";
        exit(EINVP);
      }

      m_opts.path = argv[i];
    }

    if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
      i++;

      if (i >= argc || argv[i][0] == '-') {
        param_error("Name");
        exit(EINVP);
      }

      std::regex valid_name("^\\w+$");
      std::cmatch match;
      if (!std::regex_search(argv[i], match, valid_name)) {

        std::cout << "Invalid name provided, must match ^\\w+$ (Upper & "
                     "Lower + any or no digits).\n";
        exit(EINVP);
      }

      m_opts.name = argv[i];
    }

    if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--git") == 0) {
      m_opts.has_git = true;
    }

    if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--template") == 0) {
      m_opts.use_template = true;

      i++;

      if (i >= argc) {
        param_error("Template");
        exit(EINVP);
      }

      m_opts.template_name = argv[i];

      return m_opts; // We use the template as args, early return.
    }
  }

  return m_opts;
}

std::unordered_map<std::string, std::string>
CLI::parse_opts(const CLIOpts &opts) {
  std::unordered_map<std::string, std::string> mapped_args = {
      {"PROJECT_NAME", opts.name},
      {"CXX_STANDARD", "20"},
  };

  return mapped_args;
}

#pragma once

#include <optional>
#include <string>
#include <unordered_map>

struct CLIOpts {
  bool is_tui_mode = false;
  bool has_git = false;
  std::string path;
  std::string name;

  bool use_template = false;
  std::optional<std::string> template_name;
};

class CLI {
public:
  const CLIOpts &parse(const int argc, char **argv);

  static std::unordered_map<std::string, std::string>
  parse_opts(const CLIOpts &opts);

private:
  CLIOpts m_opts;
};

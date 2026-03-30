#pragma once
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Module.hpp"

struct TargetInjection {
  std::string target_name;
  std::string module_name;
  std::vector<std::string> link;
  std::optional<std::string> cmake_include;
  std::optional<std::string> post_link;
};

struct ResolvedModule {
  std::string name;
  std::optional<FetchContent> fetch_content;
};

struct ResolvedTarget {
  TargetType type;
  std::string name;
  std::string path;
};

struct ResolvedProject {
  ProjectConfig config;
  std::vector<ResolvedTarget> targets;
  std::vector<ResolvedModule> modules;
  std::vector<TargetInjection> injections;
};

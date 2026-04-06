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
  TargetType type = TargetType::Executable;
  std::string name;
  std::string path;

  bool operator==(const ResolvedTarget &other) const {
    return name == other.name;
  }
};

// Specialization here because we need to hash for no dups
namespace std {
template <> struct hash<ResolvedTarget> {
  size_t operator()(const ResolvedTarget &target) const noexcept {
    return hash<std::string>{}(target.name);
  }
};
} // namespace std

struct ResolvedProject {
  ProjectConfig config;
  std::vector<ResolvedTarget> targets;
  std::vector<ResolvedModule> modules;
  std::vector<TargetInjection> injections;
};

#pragma once
#include "CPGen/Core/Resolved.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class ModuleResolver {
public:
  ModuleResolver();
  std::pair<ResolvedModule, std::vector<TargetInjection>>
  resolveModule(const std::string &module_name);

private:
  void resolvePaths();
  void addDevModulePath();

  std::vector<TargetInjection> readInjection(const nlohmann::json &json_obj);

  std::vector<std::filesystem::path> m_resolved_paths;
};

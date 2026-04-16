#include "CPGen/Resolvers/ModuleResolver.hpp"
#include "CPGen/Core/Resolved.hpp"
#include "CPGen/System/PathResolver.hpp"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

ModuleResolver::ModuleResolver() { resolvePaths(); }

std::pair<ResolvedModule, std::vector<TargetInjection>>
ModuleResolver::resolveModule(const std::string &module_name) {
  for (const auto &path : m_resolved_paths) {
    const auto complete_path =
        path / "modules" / module_name / (module_name + ".json");
    if (std::filesystem::exists(complete_path)) {
      return loadModule(complete_path);
    }
  }
  throw std::runtime_error("Module not found: " + module_name);
}

std::pair<ResolvedModule, std::vector<TargetInjection>>
ModuleResolver::loadModule(const std::filesystem::path &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path.string());
  }

  nlohmann::json obj;
  file >> obj;

  ResolvedModule module;
  module.name = obj.at("name").get<std::string>();
  module.fetch_content.emplace();
  module.fetch_content->module_name =
      obj.at("fetch_content").at("module_name").get<std::string>();
  module.fetch_content->url =
      obj.at("fetch_content").at("url").get<std::string>();

  return {module, readInjection(obj)};
}

std::vector<TargetInjection>
ModuleResolver::readInjection(const nlohmann::json &json_obj) {
  std::vector<TargetInjection> injections;

  for (auto it = json_obj.at("targets").begin();
       it != json_obj.at("targets").end(); ++it) {
    TargetInjection injection;
    injection.target_name = it.key();
    injection.module_name = json_obj.at("name").get<std::string>();

    const auto &target = it.value();
    injection.link = target.at("link").get<std::vector<std::string>>();

    if (target.contains("cmake_include")) {
      injection.cmake_include = target.at("cmake_include").get<std::string>();
    }
    if (target.contains("post_link")) {
      injection.post_link = target.at("post_link").get<std::string>();
    }

    injections.push_back(injection);
  }

  return injections;
}

void ModuleResolver::resolvePaths() {
  m_resolved_paths = PathResolver::resolveTemplatePaths();
}

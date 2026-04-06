#include "CPGen/Resolvers/ModuleResolver.hpp"
#include "CPGen/Core/Resolved.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unistd.h>
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
  for (const auto &path : {
           std::filesystem::path("~/.config/cpgen/templates/modules"),
           std::filesystem::path("/usr/local/share/cpgen/templates/modules"),
           std::filesystem::path("/usr/share/cpgen/templates/modules"),
       }) {
    if (std::filesystem::exists(path)) {
      m_resolved_paths.push_back(path);
    }
  }

  addDevModulePath();

  if (m_resolved_paths.empty()) {
    throw std::runtime_error("No valid path found for configuration.");
  }
}

// Dev related todo(Darleanow): Remove this.
// todo(Darleanow): Add win support
void ModuleResolver::addDevModulePath() {
  std::vector<char> buf(512);
  std::filesystem::path dev_path;

  while (true) {
    const ssize_t len = readlink("/proc/self/exe", buf.data(), buf.size() - 1);
    if (len == -1) {
      throw std::runtime_error("readlink failed");
    }
    if (len < static_cast<ssize_t>(buf.size() - 1)) {
      dev_path = std::string(buf.data(), static_cast<std::size_t>(len));
      break;
    }
    buf.resize(buf.size() * 2);
  }

  dev_path = dev_path.parent_path() / "templates";
  if (std::filesystem::exists(dev_path)) {
    m_resolved_paths.push_back(dev_path);
  }
}

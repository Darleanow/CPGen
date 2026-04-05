#include "CPGen/Resolvers/ModuleResolver.hpp"
#include "CPGen/Core/Resolved.hpp"
#include <filesystem>
#include <fstream>
#include <utility>

std::vector<std::filesystem::path> paths{
    std::filesystem::path("~/.config/cpgen/templates/modules"),
    std::filesystem::path("/usr/local/share/cpgen/templates/modules"),
    std::filesystem::path("/usr/share/cpgen/templates/modules")};

ModuleResolver::ModuleResolver() { resolvePaths(); }

std::pair<ResolvedModule, std::vector<TargetInjection>>
ModuleResolver::resolveModule(const std::string &module_name) {
  ResolvedModule module;
  std::vector<TargetInjection> injections;

  for (const auto &path : m_resolved_paths) {
    std::filesystem::path complete_path =
        path / "modules" / module_name / (module_name + ".json");

    // We search for the first version of the pkg available, first in usr
    // config, then global. We early return on the first found.
    if (std::filesystem::exists(complete_path)) {
      std::ifstream file(complete_path);

      if (!file.is_open()) {
        throw std::runtime_error("Couldnt open file for read: " +
                                 complete_path.string());
      }

      nlohmann::json obj;
      file >> obj;

      module.name = obj.at("name").get<std::string>();
      module.fetch_content.emplace();
      module.fetch_content->module_name =
          obj.at("fetch_content").at("module_name").get<std::string>();
      module.fetch_content->url =
          obj.at("fetch_content").at("url").get<std::string>();

      injections = readInjection(obj);

      return {module, injections};
    }
  }

  throw std::runtime_error("Module not found: " + module_name);
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
  for (const auto &path : paths) {
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
    ssize_t len = readlink("/proc/self/exe", buf.data(), buf.size() - 1);
    if (len == -1) {
      throw std::runtime_error("readlink failed dev");
    }

    if (len < static_cast<ssize_t>(buf.size() - 1)) {
      dev_path = std::filesystem::path(
          std::string(buf.data(), static_cast<size_t>(len)));
      break;
    }

    // todo(Darleanow): Maybe enhance this, if needed
    buf.resize(buf.size() * 2);
  }

  dev_path = dev_path.parent_path() / "templates";

  if (std::filesystem::exists(dev_path)) {
    m_resolved_paths.push_back(dev_path);
  }
}

#include "CPGen/Resolvers/ProjectResolver.hpp"
#include <unordered_set>

ProjectResolver::ProjectResolver()
    : m_module_resolver(std::make_unique<ModuleResolver>()) {}

ResolvedProject ProjectResolver::resolve(const ProjectConfig &config) {
  ResolvedProject project;

  std::unordered_set<ResolvedTarget> targets;

  project.config = config;

  for (const auto &module_name : config.modules) {
    auto resolved = m_module_resolver.get()->resolveModule(module_name);
    project.modules.push_back(resolved.first);
    for (const auto &injection : resolved.second) {
      // Manually build target here for now, later on we will accept
      // customization on targets and then we will build a target resolver.
      ResolvedTarget target;
      target.name = injection.target_name;

      if (injection.target_name == "test") {
        target.type = TargetType::Test;
        target.path =
            std::filesystem::path(config.path + config.name + "/tests");
      } else if (injection.target_name == "library") {
        target.type = TargetType::Library;
        target.path = std::filesystem::path(config.path + config.name + "/src");
      } else {
        target.type = TargetType::Executable;
        target.path = std::filesystem::path(config.path + config.name + "/src");
      }

      targets.insert(target);
    }
  }

  for (const auto &target : targets) {
    project.targets.push_back(target);
  }

  return project;
}

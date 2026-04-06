#include "CPGen/Resolvers/ProjectResolver.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Resolved.hpp"
#include "CPGen/Resolvers/ModuleResolver.hpp"
#include <memory>
#include <string>
#include <unordered_set>

ProjectResolver::ProjectResolver()
    : m_module_resolver(std::make_unique<ModuleResolver>()) {}

ResolvedProject ProjectResolver::resolve(const ProjectConfig &config) {
  ResolvedProject project;
  project.config = config;

  std::unordered_set<ResolvedTarget> seen;

  for (const auto &module_name : config.modules) {
    auto [module, injections] = m_module_resolver->resolveModule(module_name);
    project.modules.push_back(module);

    for (const auto &injection : injections) {
      if (seen.insert(buildTarget(injection, config)).second) {
        project.targets.push_back(buildTarget(injection, config));
      }
    }
  }

  return project;
}

ResolvedTarget ProjectResolver::buildTarget(const TargetInjection &injection,
                                            const ProjectConfig &config) {
  ResolvedTarget target;
  target.name = injection.target_name;

  if (injection.target_name == "test") {
    target.type = TargetType::Test;
    target.path = config.path + config.name + "/tests";
  } else if (injection.target_name == "library") {
    target.type = TargetType::Library;
    target.path = config.path + config.name + "/src";
  } else {
    target.type = TargetType::Executable;
    target.path = config.path + config.name + "/src";
  }

  return target;
}

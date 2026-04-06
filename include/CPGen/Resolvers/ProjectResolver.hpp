#pragma once
#include "CPGen/Core/Config.hpp"
#include "CPGen/Core/Resolved.hpp"
#include "CPGen/Resolvers/ModuleResolver.hpp"
#include <memory>

class ProjectResolver {
public:
  ProjectResolver();

  ResolvedProject resolve(const ProjectConfig &config);

private:
  static ResolvedTarget buildTarget(const TargetInjection &injection,
                                    const ProjectConfig &config);

  std::unique_ptr<ModuleResolver> m_module_resolver;
};

inline std::ostream &operator<<(
    std::ostream &os,
    const ResolvedProject &proj) // debug, will be removed IK OSTREAM OVERLOADS
                                 // SHALL NOT BE IN MEMBERS BUT IT WAS THE
                                 // CLEANEST VISUAL WAY OF DOING THIS !!!!!
{
  os << "Begin dump of project config\n"
     << "Name: " << proj.config.name << "\nPath: " << proj.config.path;
  os << "\nBegin dump of modules: \n";

  for (const auto &mod : proj.modules) {
    os << "Name: " << mod.name;
    if (mod.fetch_content.has_value()) {
      os << "\nFetchContent name: " << mod.fetch_content->module_name
         << "\nFetchContent URL:" << mod.fetch_content->url;
    }
  }
  os << "\nBegin dump of deduced targets: \n";
  for (const auto &target : proj.targets) {
    os << "Name: " << target.name << "\nPath: " << target.path;
  }

  return os;
}

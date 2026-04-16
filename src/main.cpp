#include "CPGen/CLI/CLI.hpp"
#include "CPGen/Core/Config.hpp"
#include "CPGen/Pipeline/CMakeEmitter.hpp"
#include "CPGen/Pipeline/GenerationPipeline.hpp"
#include "CPGen/Pipeline/ScaffoldEmitter.hpp"
#include "CPGen/Pipeline/ToolingEmitter.hpp"
#include "CPGen/Resolvers/ProjectResolver.hpp"
#include "CPGen/System/PathResolver.hpp"
#include "CPGen/System/System.hpp"
#include "CPGen/TUI/Components/Basic/Checkbox.hpp"
#include "CPGen/TUI/Components/Basic/Input.hpp"
#include "CPGen/TUI/Components/Groups/ComponentGroup.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/View/View.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace {

/// Run the generation pipeline for a fully resolved project configuration.
void generateProject(const ProjectConfig &config) {
  ProjectResolver resolver;
  auto project = resolver.resolve(config);

  const auto template_root = PathResolver::findTemplatesRoot();
  const auto output_dir =
      std::filesystem::path(project.config.path) / project.config.name;

  GenerationPipeline pipeline;
  pipeline.addEmitter(std::make_unique<CMakeEmitter>());
  pipeline.addEmitter(std::make_unique<ScaffoldEmitter>(template_root));
  pipeline.addEmitter(std::make_unique<ToolingEmitter>(template_root));
  pipeline.execute(project, output_dir);

  std::cout << "Project generated at: " << output_dir << "\n";
}

/// Launch the interactive TUI, collect user input, and return a ProjectConfig.
ProjectConfig runTui() {
  std::locale::global(std::locale(""));

  if (!System::isFontValid()) {
    std::cout << "Fallback to basic ascii output.\n";
  }

  ProjectConfig config;

  auto group =
      std::make_unique<ComponentGroup>("Project Options", Ascii::GEAR_ICON);
  group->addChild(
      std::make_unique<Checkbox>("Enable git ?", [&config](bool checked) {
        config.tooling.has_git = checked;
      }));
  group->addChild(std::make_unique<Checkbox>(
      "Enable clang-tidy ?", [&config](bool checked) {
        config.tooling.clang_tidy = checked;
      }));
  group->addChild(std::make_unique<Checkbox>(
      "Enable clang-format ?", [&config](bool checked) {
        config.tooling.clang_format = checked;
      }));

  auto name_input =
      std::make_unique<Input>("Project Name", [&config](std::string value) {
        config.name = std::move(value);
      });

  auto path_input = std::make_unique<Input>(
      "Output Path", [&config](std::string value) {
        config.path = std::move(value);
      });

  View view;
  view.addSection(std::move(group));
  view.addSection(std::move(name_input));
  view.addSection(std::move(path_input));
  view.run();

  // Default output path to current directory if not specified
  if (config.path.empty()) {
    config.path = std::filesystem::current_path().string();
  }

  return config;
}

} // namespace

int main(int argc, char **argv) {
  try {
    CLI cli;
    auto opts = cli.parse(argc, argv);

    if (std::holds_alternative<ProjectConfig>(opts)) {
      generateProject(std::get<ProjectConfig>(opts));
    } else {
      auto config = runTui();

      if (config.name.empty()) {
        std::cerr << "Error: No project name provided.\n";
        return 1;
      }

      generateProject(config);
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

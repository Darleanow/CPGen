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

int main(int argc, char **argv) {
  try {
    CLI cli;
    auto opts = cli.parse(argc, argv);

    if (std::holds_alternative<ProjectConfig>(opts)) {
      ProjectResolver resolver;
      auto project = resolver.resolve(std::get<ProjectConfig>(opts));

      const auto template_root = PathResolver::findTemplatesRoot();
      const auto output_dir = std::filesystem::path(project.config.path) /
                              project.config.name;

      GenerationPipeline pipeline;
      pipeline.addEmitter(std::make_unique<CMakeEmitter>());
      pipeline.addEmitter(std::make_unique<ScaffoldEmitter>(template_root));
      pipeline.addEmitter(std::make_unique<ToolingEmitter>(template_root));
      pipeline.execute(project, output_dir);

      std::cout << "Project generated at: " << output_dir << "\n";
      return 0;
    }

    std::locale::global(std::locale(""));

    if (!System::isFontValid()) {
      std::cout << "Fallback to basic ascii output.\n";
    }

    ProjectOptions options;

    auto group =
        std::make_unique<ComponentGroup>("Project Options", Ascii::GEAR_ICON);
    group->addChild(
        std::make_unique<Checkbox>("Enable git ?", [&options](bool checked) {
          options.has_git = checked;
        }));
    group->addChild(
        std::make_unique<Checkbox>("Use template ?", [&options](bool checked) {
          options.use_template = checked;
        }));

    auto input =
        std::make_unique<Input>("Project Name", [&options](std::string value) {
          options.name = std::move(value);
        });

    View view;
    view.addSection(std::move(group));
    view.addSection(std::move(input));
    view.run();

    std::cout << "Project: " << options.name << "\n";
    std::cout << "Git: " << (options.has_git ? "yes" : "no") << "\n";
    std::cout << "Template: " << (options.use_template ? "yes" : "no") << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

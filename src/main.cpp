#include "CPGen/CLI/CLI.hpp"
#include "CPGen/Resolvers/ProjectResolver.hpp"
#include "CPGen/System/System.hpp"
#include "CPGen/TUI/Components/Basic/Checkbox.hpp"
#include "CPGen/TUI/Components/Basic/Input.hpp"
#include "CPGen/TUI/Components/Groups/ComponentGroup.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/View/View.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  CLI cli;
  auto opts = cli.parse(argc, argv);

  auto result = std::visit(
      [](auto opt) -> bool {
        if constexpr (std::is_same_v<decltype(opt), ProjectConfig>) {
          // We just need to gen project
          return true;
        }
        return false;
      },
      opts);

  if (result == true) {
    ProjectResolver resolver;
    auto res = resolver.resolve(std::get<ProjectConfig>(opts));
    std::cout << res << std::endl;
    return 0; // Program finished its execution.
  }

  std::setlocale(LC_ALL, "");

  if (!System::isFontValid()) {
    std::cout << "Fallback to basic ascii output.\n";
  }

  ProjectOptions options;

  // Build the UI
  auto group =
      std::make_unique<ComponentGroup>("Project Options", Ascii::GearIcon);
  group->addChild(std::make_unique<Checkbox>(
      "Enable git ?", [&options](bool checked) { options.has_git = checked; }));
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

  return 0;
}

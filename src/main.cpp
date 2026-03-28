#include "CPGen/CLI/CLI.hpp"
#include "CPGen/ProjectGenerator/ProjectGenerator.hpp"
#include "CPGen/System/System.hpp"
#include "CPGen/TUI/Components/Basic/Checkbox.hpp"
#include "CPGen/TUI/Components/Basic/Input.hpp"
#include "CPGen/TUI/Components/Groups/ComponentGroup.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/View/View.hpp"
#include <locale>

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  CLI cli;
  auto &opts = cli.parse(argc, argv);
  ProjectGenerator generator(opts);

  if (!opts.is_tui_mode) {
    generator.generateProject();
    return 0;
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

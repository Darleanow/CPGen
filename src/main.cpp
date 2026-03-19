#include "CPGen/CLI/CLI.hpp"
#include "CPGen/ProjectGenerator/ProjectGenerator.hpp"
#include "CPGen/System/System.hpp"
#include "CPGen/TUI/Components/Basic/Input.hpp"
#include "CPGen/TUI/Components/Groups/CheckboxGroup.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/View/MainView.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  CLI cli;
  auto &opts = cli.parse(argc, argv);
  ProjectGenerator generator(opts);

  if (!opts.is_tui_mode) {
    generator.generateProject();
  }

  std::system("clear");

  if (!System::isFontValid()) {
    std::cout << "Fallback to basic ascii output.\n";
  }

  std::system("clear");

  MainView view;
  CLIOpts options;

  auto group = std::make_unique<CheckboxGroup>(
      "Testname", std::vector<std::string>({"Enable git ?", "Option 2"}),
      std::vector<std::function<void(bool)>>(
          {[&options](bool checked) { options.has_git = checked; },
           [&options](bool checked) { options.name = checked ? "" : ""; }}),
      Ascii::CppIcon);

  auto input = std::make_unique<Input>(
      "Name", [&options](std::string value) { options.name = value; });

  view.addComponent(std::move(group));
  view.addComponent(std::move(input));
  view.render();

  std::cout << options.has_git << "\n";

  return 0;
}

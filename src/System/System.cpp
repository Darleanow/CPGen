#include "CPGen/System/System.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include <iostream>

namespace System {

bool isFontValid() {
  std::string ans;

  std::cout << "Testing glyphs (font support): " << Ascii::CheckboxUnchecked
            << " " << Ascii::CheckboxChecked << "\n";
  std::cout << "Does this looks like two checkboxes ? [y/n]: ";

  std::cin >> ans;

  return (ans == "y" || ans == "Y");
}

}; // namespace System

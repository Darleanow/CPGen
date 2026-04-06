#include "CPGen/System/System.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include <iostream>
#include <string>

namespace System {

bool isFontValid() {
  std::string ans;

  std::cout << "Testing glyphs (font support): " << Ascii::CHECKBOX_UNCHECKED
            << " " << Ascii::CHECKBOX_CHECKED << "\n";
  std::cout << "Does this looks like two checkboxes ? [y/n]: ";

  std::cin >> ans;

  return (ans == "y" || ans == "Y");
}

}; // namespace System

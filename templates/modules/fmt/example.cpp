#include <fmt/core.h>
#include <fmt/color.h>

int main() {
  fmt::print("Hello, {}!\n", "world");
  fmt::print(fg(fmt::color::green), "Formatted with colour!\n");

  return 0;
}

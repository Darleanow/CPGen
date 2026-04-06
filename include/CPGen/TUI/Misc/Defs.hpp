#pragma once

#include <cstdint>
#include <variant>

namespace Defs {

enum class Special : std::uint8_t {
  Enter,
  Up,
  Down,
  Left,
  Right,
  Escape,
  Backspace,
  Tab
};
using Key = std::variant<char, Special>;

} // namespace Defs

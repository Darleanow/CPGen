#pragma once

#include <variant>

namespace Defs {

enum class Special { Enter, Up, Down, Left, Right, Escape, Backspace, Tab };
using Key = std::variant<char, Special>;

} // namespace Defs

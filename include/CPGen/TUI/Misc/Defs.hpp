#pragma once

/**
 * @file Defs.hpp
 * @brief Shared type definitions for the TUI input system.
 * @author Darleanow
 */

#include <cstdint>
#include <variant>

/**
 * @namespace Defs
 * @brief Core type definitions shared across all TUI components.
 */
namespace Defs {

/**
 * @enum Special
 * @brief Non-printable / control keys recognised by the input reader.
 *
 * The underlying type is @c std::uint8_t to keep the enum as compact as
 * possible (see the @c performance-enum-size guideline).
 */
enum class Special : std::uint8_t {
  Enter,     ///< Return / Enter key (@c \\r or @c \\n).
  Up,        ///< Up arrow (@c ESC [ A).
  Down,      ///< Down arrow (@c ESC [ B).
  Left,      ///< Left arrow (@c ESC [ C).
  Right,     ///< Right arrow (@c ESC [ D).
  Escape,    ///< Escape key or unrecognised escape sequence.
  Backspace, ///< Backspace (@c 0x7F) or Delete (@c 0x08).
  Tab,       ///< Horizontal tab (@c \\t).
};

/**
 * @typedef Key
 * @brief A single keypress from stdin, represented as either a printable character
 *        or a @ref Special control key.
 *
 * Components receive a @c Key value from @c Utils::readKey() and pattern-match
 * it with @c std::holds_alternative / @c std::get to implement their input logic.
 */
using Key = std::variant<char, Special>;

} // namespace Defs

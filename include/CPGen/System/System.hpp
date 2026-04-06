#pragma once

/**
 * @file System.hpp
 * @brief System-level utilities for probing the runtime environment.
 * @author Darleanow
 */

/**
 * @namespace System
 * @brief Utilities that query or interact with the host operating system.
 */
namespace System {

/**
 * @brief Interactively check whether the terminal font supports Nerd Font
 * glyphs.
 *
 * Renders a pair of checkbox glyphs to @c stdout and prompts the user to
 * confirm whether they render correctly.  This is used at startup to decide
 * whether the TUI should use Unicode icons or fall back to plain ASCII.
 *
 * @return @c true if the user confirmed the glyphs look correct (@c y / @c Y),
 *         @c false otherwise.
 *
 * @note This function reads from @c std::cin and is therefore not testable
 *       without stdin redirection.
 */
bool isFontValid();

} // namespace System

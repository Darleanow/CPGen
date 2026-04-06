#pragma once

/**
 * @file Utils.hpp
 * @brief TUI utility functions: ANSI colour helpers, input reading, and string
 * metrics.
 * @author Darleanow
 *
 * All functions in this file are @c inline and live in header-only namespaces
 * so they can be included by any TUI translation unit without ODR issues.
 *
 * ### Colour usage
 * Build an ANSI escape sequence with @ref Utils::Colours::esc and combine it
 * with
 * @ref Utils::Colours::RESET to restore the default style:
 * @code
 *   std::string s = Utils::Colours::esc(Utils::Colours::FG_GREEN)
 *                 + "hello"
 *                 + Utils::Colours::esc(Utils::Colours::RESET);
 * @endcode
 */

#include "Misc/Defs.hpp"
#include <array>
#include <cwchar>
#include <string>
#include <unordered_map>

#ifndef _WIN32
#include <poll.h>
#include <unistd.h>
#endif

/**
 * @namespace Utils
 * @brief TUI utility functions and constants.
 */
namespace Utils {

/**
 * @namespace Utils::Colours
 * @brief ANSI SGR (Select Graphic Rendition) code constants and escape sequence
 * builder.
 *
 * Each constant is the numeric SGR parameter string (without the surrounding
 * @c ESC[…m).  Pass them to @ref esc to get a ready-to-print escape sequence.
 */
namespace Colours {

/// @name Standard foreground colours
/// @{
inline constexpr const char *FG_BLACK = "30";   ///< Black foreground.
inline constexpr const char *FG_RED = "31";     ///< Red foreground.
inline constexpr const char *FG_GREEN = "32";   ///< Green foreground.
inline constexpr const char *FG_YELLOW = "33";  ///< Yellow foreground.
inline constexpr const char *FG_BLUE = "34";    ///< Blue foreground.
inline constexpr const char *FG_MAGENTA = "35"; ///< Magenta foreground.
inline constexpr const char *FG_CYAN = "36";    ///< Cyan foreground.
inline constexpr const char *FG_WHITE = "37";   ///< White foreground.
/// @}

/// @name Bright / light foreground colours
/// @{
inline constexpr const char *FG_BBLACK =
    "90"; ///< Bright black (dark grey) foreground.
inline constexpr const char *FG_BRED = "91";     ///< Bright red foreground.
inline constexpr const char *FG_BGREEN = "92";   ///< Bright green foreground.
inline constexpr const char *FG_BYELLOW = "93";  ///< Bright yellow foreground.
inline constexpr const char *FG_BBLUE = "94";    ///< Bright blue foreground.
inline constexpr const char *FG_BMAGENTA = "95"; ///< Bright magenta foreground.
inline constexpr const char *FG_BCYAN = "96";    ///< Bright cyan foreground.
inline constexpr const char *FG_BWHITE = "97";   ///< Bright white foreground.
/// @}

/// @name Standard background colours
/// @{
inline constexpr const char *BG_BLACK = "40";   ///< Black background.
inline constexpr const char *BG_RED = "41";     ///< Red background.
inline constexpr const char *BG_GREEN = "42";   ///< Green background.
inline constexpr const char *BG_YELLOW = "43";  ///< Yellow background.
inline constexpr const char *BG_BLUE = "44";    ///< Blue background.
inline constexpr const char *BG_MAGENTA = "45"; ///< Magenta background.
inline constexpr const char *BG_CYAN = "46";    ///< Cyan background.
inline constexpr const char *BG_WHITE = "47";   ///< White background.
/// @}

/// @name Bright / light background colours
/// @{
inline constexpr const char *BG_BBLACK = "100";  ///< Bright black background.
inline constexpr const char *BG_BRED = "101";    ///< Bright red background.
inline constexpr const char *BG_BGREEN = "102";  ///< Bright green background.
inline constexpr const char *BG_BYELLOW = "103"; ///< Bright yellow background.
inline constexpr const char *BG_BBLUE = "104";   ///< Bright blue background.
inline constexpr const char *BG_BMAGENTA =
    "105";                                      ///< Bright magenta background.
inline constexpr const char *BG_BCYAN = "106";  ///< Bright cyan background.
inline constexpr const char *BG_BWHITE = "107"; ///< Bright white background.
/// @}

/// @name Text style modifiers
/// @{
inline constexpr const char *RESET = "0";      ///< Reset all attributes.
inline constexpr const char *BOLD = "1";       ///< Bold / increased intensity.
inline constexpr const char *DIM = "2";        ///< Dim / decreased intensity.
inline constexpr const char *ITALIC = "3";     ///< Italic.
inline constexpr const char *UNDERLINE = "4";  ///< Underline.
inline constexpr const char *BLINK = "5";      ///< Slow blink.
inline constexpr const char *INVERSE = "7";    ///< Reverse video (swap FG/BG).
inline constexpr const char *HIDDEN = "8";     ///< Conceal / invisible.
inline constexpr const char *STRIKETHRU = "9"; ///< Strikethrough.
/// @}

/**
 * @brief Build a complete ANSI SGR escape sequence from a code string.
 *
 * Wraps @p code in the standard @c ESC [ … m envelope so it can be embedded
 * directly in output strings.
 *
 * @param code An SGR parameter string such as @ref FG_GREEN or @ref BOLD.
 * @return A ready-to-print ANSI escape sequence (e.g. @c "\\033[32m").
 */
inline std::string esc(const char *code) {
  return "\033[" + std::string(code) + "m";
}

} // namespace Colours

/**
 * @brief Poll stdin for available data within a timeout.
 *
 * Uses @c poll(2) on POSIX to avoid blocking indefinitely when checking for
 * the second byte of a multi-byte escape sequence.  Always returns @c true on
 * Windows (where @c poll is not readily available).
 *
 * @param timeout_ms Maximum wait time in milliseconds.  Pass @c 0 for a
 *        non-blocking check.
 * @return @c true if at least one byte is ready to read from @c STDIN_FILENO.
 */
inline bool stdinReady(int timeout_ms) {
#ifndef _WIN32
  struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
  return poll(&pfd, 1, timeout_ms) > 0;
#else
  (void)timeout_ms;
  return true;
#endif
}

/**
 * @brief Remove all ANSI escape sequences from a string.
 *
 * Scans @p s character-by-character and discards everything between an
 * @c ESC (@c 0x1B) and the subsequent alphabetic terminator.
 *
 * @param s A string that may contain @c ESC[…m ANSI colour/style codes.
 * @return The plain-text content of @p s with all escape sequences stripped.
 */
inline std::string stripAnsi(const std::string &s) {
  std::string result;
  result.reserve(s.size());
  bool in_escape = false;
  for (const char c : s) {
    if (in_escape && std::isalpha(c) != 0) {
      in_escape = false;
      continue;
    }
    if (in_escape) {
      continue;
    }
    if (c == '\033') {
      in_escape = true;
      continue;
    }
    result += c;
  }
  return result;
}

/**
 * @brief Compute the visual (display) column width of a UTF-8 string.
 *
 * Strips ANSI escape codes first, then converts the result to a wide-character
 * string and calls POSIX @c wcswidth(3) to obtain the number of terminal
 * columns the string occupies (accounting for double-width CJK characters,
 * zero-width combining marks, etc.).
 *
 * @param s A UTF-8 string, possibly containing ANSI escape sequences.
 * @return Number of terminal columns the visible content of @p s occupies.
 *         Falls back to the character count if @c wcswidth returns a negative
 *         value (e.g. when the string contains non-printable wide characters).
 *
 * @note Currently POSIX-only.  A cross-platform alternative using utf8proc
 *       is planned (see the TODO comment in the implementation).
 */
inline size_t visualWidth(const std::string &s) {
  const std::string stripped = stripAnsi(s);
  std::wstring wide(stripped.size(), L'\0');
  (void)std::mbstowcs(wide.data(), stripped.c_str(), stripped.size());
  wide.resize(std::wcslen(wide.c_str()));

  // todo(Darleanow): Make this monstruosity multiplatform (aint have this on
  // win) Considering using the utf8proc
  const int w = ::wcswidth(wide.c_str(), wide.size()); // POSIX
  return w < 0 ? wide.size() : static_cast<size_t>(w);
}

/**
 * @brief Decode an escape sequence whose leading @c ESC byte has already been
 * consumed.
 *
 * Polls stdin for the next byte (50 ms timeout).  If it reads @c '[' it polls
 * again for the final letter and maps it to an arrow key via a static lookup
 * table. Any unrecognised sequence or timeout returns @ref
 * Defs::Special::Escape.
 *
 * @return The decoded @ref Defs::Key — one of @ref Defs::Special::Up,
 *         @ref Defs::Special::Down, @ref Defs::Special::Left,
 *         @ref Defs::Special::Right, or @ref Defs::Special::Escape.
 */
inline Defs::Key handleEscapeKey() {
  if (!stdinReady(/*timeout_ms=*/50)) {
    return Defs::Special::Escape;
  }

  std::array<char, 2> seq = {};
  [[maybe_unused]] const auto seq0 = read(STDIN_FILENO, seq.data(), 1);

  if (seq[0] != '[') {
    return Defs::Special::Escape;
  }

  if (!stdinReady(/*timeout_ms=*/50)) {
    return Defs::Special::Escape;
  }

  [[maybe_unused]] const auto seq1 = read(STDIN_FILENO, seq.data() + 1, 1);

  static const std::unordered_map<char, Defs::Special> ARROWS = {
      {'A', Defs::Special::Up},
      {'B', Defs::Special::Down},
      {'C', Defs::Special::Right},
      {'D', Defs::Special::Left}};

  if (auto it = ARROWS.find(seq[1]); it != ARROWS.end()) {
    return it->second;
  }

  return Defs::Special::Escape;
}

/**
 * @brief Read a single keypress from stdin (terminal must be in raw mode).
 *
 * Reads one byte and maps it to the appropriate @ref Defs::Key variant:
 * - @c \\r / @c \\n → @ref Defs::Special::Enter
 * - @c 0x1B (ESC)   → delegates to @ref handleEscapeKey
 * - @c 0x7F / @c 0x08 → @ref Defs::Special::Backspace
 * - @c \\t           → @ref Defs::Special::Tab
 * - Any other byte  → @c char variant
 *
 * @return The decoded @ref Defs::Key.
 *
 * @note Requires an active @ref Terminal instance (raw mode + cursor hidden).
 *       The return value of the underlying @c read(2) call is intentionally
 *       discarded; errors are left to the caller's event loop to detect via
 *       subsequent @c stdinReady checks.
 */
inline Defs::Key readKey() {
  std::array<char, 3> buf = {};
  [[maybe_unused]] const auto buf0 = read(STDIN_FILENO, buf.data(), 1);

  if (buf[0] == '\r' || buf[0] == '\n') {
    return Defs::Special::Enter;
  }
  if (buf[0] == 27) {
    return handleEscapeKey();
  }
  if (buf[0] == 127 || buf[0] == 8) {
    return Defs::Special::Backspace;
  }
  if (buf[0] == '\t') {
    return Defs::Special::Tab;
  }

  return buf[0];
}

} // namespace Utils

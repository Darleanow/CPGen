#pragma once

#include "Misc/Defs.hpp"
#include <array>
#include <cwchar>
#include <string>
#include <unordered_map>

#ifndef _WIN32
#include <poll.h>
#include <unistd.h>
#endif

namespace Utils {

namespace Colours {

// Foreground (text)
inline constexpr const char *FG_BLACK = "30";
inline constexpr const char *FG_RED = "31";
inline constexpr const char *FG_GREEN = "32";
inline constexpr const char *FG_YELLOW = "33";
inline constexpr const char *FG_BLUE = "34";
inline constexpr const char *FG_MAGENTA = "35";
inline constexpr const char *FG_CYAN = "36";
inline constexpr const char *FG_WHITE = "37";

// Bright / Light foreground
inline constexpr const char *FG_BBLACK = "90";
inline constexpr const char *FG_BRED = "91";
inline constexpr const char *FG_BGREEN = "92";
inline constexpr const char *FG_BYELLOW = "93";
inline constexpr const char *FG_BBLUE = "94";
inline constexpr const char *FG_BMAGENTA = "95";
inline constexpr const char *FG_BCYAN = "96";
inline constexpr const char *FG_BWHITE = "97";

// Background
inline constexpr const char *BG_BLACK = "40";
inline constexpr const char *BG_RED = "41";
inline constexpr const char *BG_GREEN = "42";
inline constexpr const char *BG_YELLOW = "43";
inline constexpr const char *BG_BLUE = "44";
inline constexpr const char *BG_MAGENTA = "45";
inline constexpr const char *BG_CYAN = "46";
inline constexpr const char *BG_WHITE = "47";

// Bright / Light background
inline constexpr const char *BG_BBLACK = "100";
inline constexpr const char *BG_BRED = "101";
inline constexpr const char *BG_BGREEN = "102";
inline constexpr const char *BG_BYELLOW = "103";
inline constexpr const char *BG_BBLUE = "104";
inline constexpr const char *BG_BMAGENTA = "105";
inline constexpr const char *BG_BCYAN = "106";
inline constexpr const char *BG_BWHITE = "107";

// Styles
inline constexpr const char *RESET = "0";
inline constexpr const char *BOLD = "1";
inline constexpr const char *DIM = "2";
inline constexpr const char *ITALIC = "3";
inline constexpr const char *UNDERLINE = "4";
inline constexpr const char *BLINK = "5";
inline constexpr const char *INVERSE = "7";
inline constexpr const char *HIDDEN = "8";
inline constexpr const char *STRIKETHRU = "9";

// Helper to build ANSI escape sequence
inline std::string esc(const char *code) {
  return "\033[" + std::string(code) + "m";
}

} // namespace Colours

/**
 * @brief Check if stdin has data available within a timeout
 * @param timeout_ms Timeout in milliseconds (0 = non-blocking check)
 * @return true if data is available to read
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
 * @brief Strip ANSI escape sequences from a string
 * @param s The string potentially containing ANSI codes
 * @return Plain text with all escape sequences removed
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
 * @brief Compute the visual (display) width of a UTF-8 string,
 *        stripping ANSI escape sequences.
 * @param s The string potentially containing ANSI codes
 * @return The number of terminal columns the string occupies
 */
inline size_t visualWidth(const std::string &s) {
  const std::string stripped = stripAnsi(s);
  std::wstring wide(stripped.size(), L'\0');
  (void)std::mbstowcs(wide.data(), stripped.c_str(), stripped.size());
  wide.resize(std::wcslen(wide.c_str()));

  // todo(Darleanow): Make this monstruosity multiplatform (aint have this on
  // win)
  // Considering using the utf8proc
  const int w = ::wcswidth(wide.c_str(), wide.size()); // POSIX
  return w < 0 ? wide.size() : static_cast<size_t>(w);
}

/**
 * @brief Handle an ESC byte already read from stdin
 * @return The key decoded from the escape sequence
 */
inline Defs::Key handleEscapeKey() {
  if (!stdinReady(/*timeout_ms=*/50)) {
    return Defs::Special::Escape;
  }

  std::array<char, 2> seq = {};
  [[maybe_unused]] const auto seq0 = read(STDIN_FILENO, seq.data(), 1); // because implem of read changes with optimization level

  if (seq[0] != '[') {
    return Defs::Special::Escape;
  }

  if (!stdinReady(/*timeout_ms=*/50)) {
    return Defs::Special::Escape;
  }

  [[maybe_unused]] const auto seq1 = read(STDIN_FILENO, seq.data() + 1, 1); // because implem of read changes with optimization level

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
 * @brief Read a single key from stdin (raw mode must be active)
 * @return The key as a Defs::Key variant
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

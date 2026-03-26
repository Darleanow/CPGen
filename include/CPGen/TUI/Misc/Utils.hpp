#pragma once

#include "CPGen/TUI/Misc/Defs.hpp"
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
 * @brief Compute the visual (display) width of a UTF-8 string,
 *        stripping ANSI escape sequences.
 * @param s The string potentially containing ANSI codes
 * @return The number of terminal columns the string occupies
 */
inline size_t visualWidth(const std::string &s) {
  std::string stripped;
  stripped.reserve(s.size());

  bool in_escape = false;
  for (char c : s) {
    if (in_escape) {
      if (std::isalpha(c))
        in_escape = false;
    } else if (c == '\033') {
      in_escape = true;
    } else {
      stripped += c;
    }
  }

  std::wstring wide(stripped.size(), L'\0');
  std::mbstowcs(wide.data(), stripped.c_str(), stripped.size());
  wide.resize(std::wcslen(wide.c_str()));

  // todo(Darleanow): Make this monstruosity multiplatform (aint have this on
  // win)
  // Considering using the utf8proc
  const int w = ::wcswidth(wide.c_str(), wide.size()); // POSIX
  return w < 0 ? wide.size() : static_cast<size_t>(w);
}

/**
 * @brief Read a single key from stdin (raw mode must be active)
 * @return The key as a Defs::Key variant
 */
inline Defs::Key readKey() {
  char buf[3] = {};
  read(STDIN_FILENO, buf, 1);

  if (buf[0] == '\r' || buf[0] == '\n')
    return Defs::Special::Enter;
  if (buf[0] == 27) {
    // Wait briefly to see if more bytes follow (escape sequence)
    // Arrow keys etc. send ESC [ X in quick succession;
    // a bare Escape press sends only ESC with no follow-up.
    if (!stdinReady(50))
      return Defs::Special::Escape; // Bare escape — return immediately

    char seq[2] = {};
    read(STDIN_FILENO, seq, 1);

    if (seq[0] == '[') {
      if (stdinReady(50)) {
        read(STDIN_FILENO, seq + 1, 1);
        static const std::unordered_map<char, Defs::Special> arrows = {
            {'A', Defs::Special::Up},
            {'B', Defs::Special::Down},
            {'C', Defs::Special::Right},
            {'D', Defs::Special::Left}};

        if (auto it = arrows.find(seq[1]); it != arrows.end())
          return it->second;
      }
    }

    return Defs::Special::Escape;
  }
  if (buf[0] == 127 || buf[0] == 8)
    return Defs::Special::Backspace;
  if (buf[0] == '\t')
    return Defs::Special::Tab;

  return buf[0]; // Regular character
}

} // namespace Utils

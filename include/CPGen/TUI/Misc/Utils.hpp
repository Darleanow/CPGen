#pragma once
#include <cstdlib>
#include <string>

namespace Utils {

namespace Colours {

// Foreground (texte)
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

} // namespace Utils

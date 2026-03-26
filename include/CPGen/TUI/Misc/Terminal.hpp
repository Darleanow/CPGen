#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include <csignal>
#include <cstdio>
#include <cstdlib>

/**
 * @class Terminal
 * @brief RAII raw-mode terminal handler
 *
 * Activates raw mode on construction, restores the original
 * terminal state on destruction. Hides cursor while active.
 * Hooks SIGINT/SIGTERM to guarantee terminal restoration.
 */
class Terminal {
public:
  Terminal() {
#ifndef _WIN32
    tcgetattr(STDIN_FILENO, &m_original);
    s_original = m_original;

    struct termios raw = m_original;
    raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
#endif
    std::fputs("\033[?25l", stdout); // Hide cursor
    std::fflush(stdout);

    // Hook signals so Ctrl+C / kill always restores the terminal
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
  }

  ~Terminal() { restore(); }

  Terminal(const Terminal &) = delete;
  Terminal &operator=(const Terminal &) = delete;

private:
  static void restore() {
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &s_original);
#endif
    std::fputs("\033[?25h", stdout); // Show cursor
    std::fflush(stdout);
  }

  static void signalHandler(int /*sig*/) {
    restore();
    std::_Exit(130); // 128 + SIGINT(2), standard convention
  }

#ifndef _WIN32
  struct termios m_original{};
  static inline struct termios s_original{};
#endif
};

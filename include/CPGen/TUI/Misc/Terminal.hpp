#pragma once

/**
 * @file Terminal.hpp
 * @brief RAII wrapper that manages terminal raw mode for the TUI.
 * @author Darleanow
 *
 * Constructing a @ref Terminal puts stdin into raw mode (no line buffering,
 * no echo) so that key presses can be read one character at a time.  The
 * destructor unconditionally restores the original terminal settings.
 *
 * Signal handlers for @c SIGINT and @c SIGTERM are installed to guarantee that
 * the terminal is restored even when the user presses @c Ctrl+C or the process
 * is killed externally.
 */

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
 * @brief RAII raw-mode terminal handler.
 *
 * ### Lifecycle
 * | Event              | Action                                         |
 * |--------------------|------------------------------------------------|
 * | Construction       | Save @c termios, enable raw mode, hide cursor, install
 * signal handlers. | | Destruction        | Restore @c termios, show cursor. |
 * | @c SIGINT/SIGTERM  | Restore terminal, call @c std::_Exit(130).     |
 *
 * @warning Only one @c Terminal instance should be alive at a time; the signal
 *          handler uses a static copy of the original @c termios structure.
 *
 * The class is non-copyable and non-movable to prevent double-restore bugs.
 */
class Terminal {
public:
  /**
   * @brief Enter raw mode and hide the cursor.
   *
   * On POSIX: reads the current @c termios, disables @c ICANON (line buffering)
   * and @c ECHO, and applies the modified struct with @c TCSANOW.
   * On all platforms: hides the cursor with @c ESC[?25l and installs signal
   * handlers for @c SIGINT and @c SIGTERM.
   */
  Terminal() {
#ifndef _WIN32
    (void)tcgetattr(STDIN_FILENO, &m_original);
    s_original = m_original;

    struct termios raw = m_original;
    raw.c_lflag &=
        ~(static_cast<tcflag_t>(ICANON) | static_cast<tcflag_t>(ECHO));
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &raw);
#endif
    (void)std::fputs("\033[?25l", stdout); // Hide cursor
    (void)std::fflush(stdout);

    // Hook signals so Ctrl+C / kill always restores the terminal
    (void)std::signal(SIGINT, signalHandler);
    (void)std::signal(SIGTERM, signalHandler);
  }

  /**
   * @brief Restore the original terminal state and show the cursor.
   */
  ~Terminal() { restore(); }

  Terminal(const Terminal &) = delete;            ///< Non-copyable.
  Terminal &operator=(const Terminal &) = delete; ///< Non-copyable.
  Terminal(Terminal &&) = delete;                 ///< Non-movable.
  Terminal &operator=(Terminal &&) = delete;      ///< Non-movable.

private:
  /**
   * @brief Restore @c termios and make the cursor visible again.
   *
   * Called both from the destructor and from @ref signalHandler, so it must be
   * @c static and safe to call from a signal context (only async-signal-safe
   * operations are performed).
   */
  static void restore() {
#ifndef _WIN32
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &s_original);
#endif
    (void)std::fputs("\033[?25h", stdout); // Show cursor
    (void)std::fflush(stdout);
  }

  /**
   * @brief Signal handler installed for @c SIGINT and @c SIGTERM.
   *
   * Restores the terminal and exits with code 130 (the POSIX convention for
   * "terminated by SIGINT": 128 + signal number).
   *
   * @param sig The signal number (unused; the same action is taken for all
   * hooked signals).
   */
  static void signalHandler(int /*sig*/) {
    restore();
    std::_Exit(130);
  }

#ifndef _WIN32
  struct termios m_original{}; ///< Per-instance copy of the original @c termios
                               ///< (used on destruction).
  static inline struct termios
      s_original{}; ///< Static copy shared with the signal handler.
#endif
};

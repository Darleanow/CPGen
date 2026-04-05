#pragma once

#include "Misc/Defs.hpp"
#include <string>

/**
 * @class Component
 * @brief Abstract base for all TUI components
 *
 * Provides the common interface: render, handle input, and focus.
 * Every component must implement render() and handleInput().
 */
class Component {
public:
  virtual ~Component() = default;

  /**
   * @brief Render the component as an ANSI string
   * @return The rendered string (may contain newlines)
   */
  virtual std::string render() const = 0;

  /**
   * @brief Handle a key press
   * @param key The key event
   * @return true if the key was consumed, false to bubble up
   */
  virtual bool handleInput(Defs::Key key) = 0;

  /**
   * @brief Set the focus state of this component
   * @param focused Whether this component should be focused
   */
  virtual void setFocused(bool focused) { m_is_focused = focused; }

  /**
   * @brief Check whether this component is focused
   * @return true if focused
   */
  bool isFocused() const { return m_is_focused; }

protected:
  bool m_is_focused = false;
};

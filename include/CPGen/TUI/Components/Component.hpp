#pragma once

/**
 * @file Component.hpp
 * @brief Abstract base class for all TUI components.
 * @author Darleanow
 *
 * Every interactive element displayed by the TUI (checkboxes, text inputs,
 * groups, …) inherits from @ref Component and implements the three virtual
 * methods: @ref render, @ref handleInput, and optionally @ref setFocused.
 *
 * ### Ownership model
 * Components are exclusively owned via @c std::unique_ptr.  The non-copyable,
 * non-movable constraint enforced by the deleted special members reflects this:
 * once a component is constructed and transferred to its owner (e.g. a
 * @ref ComponentGroup or @ref View), it must not be copied or moved.
 */

#include "Misc/Defs.hpp"
#include <string>

/**
 * @class Component
 * @brief Abstract base for all TUI widgets.
 *
 * Defines the minimal interface that the @ref View and @ref ComponentGroup
 * containers depend on to render and route input events.
 *
 * Subclasses must implement:
 * - @ref render() — return the full ANSI-formatted string representation.
 * - @ref handleInput() — process one keypress and return whether it was
 * consumed.
 *
 * Subclasses may override:
 * - @ref setFocused() — react to focus changes (e.g. repaint with a different
 * colour).
 */
class Component {
public:
  Component() = default;

  /**
   * @brief Virtual destructor — components are always owned via base-class
   * pointer.
   */
  virtual ~Component() = default;

  Component(const Component &) = delete; ///< Non-copyable (unique ownership).
  Component &
  operator=(const Component &) = delete; ///< Non-copyable (unique ownership).
  Component(Component &&) = delete;      ///< Non-movable.
  Component &operator=(Component &&) = delete; ///< Non-movable.

  /**
   * @brief Produce the full ANSI-formatted rendering of this component.
   *
   * The returned string may contain newlines and ANSI escape codes.  Callers
   * should not assume a specific line count.
   *
   * @return String suitable for direct output to a terminal.
   */
  [[nodiscard]] virtual std::string render() const = 0;

  /**
   * @brief Process a single key event.
   *
   * @param key The key decoded by @ref Utils::readKey.
   * @return @c true if the component consumed the key (no further propagation
   *         is needed), @c false if the event should bubble up to the parent.
   */
  virtual bool handleInput(Defs::Key key) = 0;

  /**
   * @brief Change the focus state of this component.
   *
   * The default implementation sets @ref m_is_focused and is sufficient for
   * most leaf components.  Containers such as @ref ComponentGroup override this
   * to also update their currently focused child.
   *
   * @param focused @c true to grant focus, @c false to remove it.
   */
  virtual void setFocused(bool focused) { m_is_focused = focused; }

  /**
   * @brief Return whether this component currently has keyboard focus.
   * @return @c true if focused.
   */
  [[nodiscard]] bool isFocused() const { return m_is_focused; }

protected:
  bool m_is_focused =
      false; ///< Current focus state, managed by @ref setFocused.
};

#pragma once

/**
 * @file Input.hpp
 * @brief A single-line text entry widget.
 * @author Darleanow
 */

#include "Components/Component.hpp"
#include <functional>
#include <string>

/**
 * @class Input
 * @brief A single-line text input component.
 *
 * Renders a fixed-width bordered box containing a label on the left and the
 * current value on the right.  When focused, the box is highlighted with a
 * distinct colour.  The component accepts printable character input, handles
 * @ref Defs::Special::Backspace for deletion, and fires @c on_change on every
 * modification.
 *
 * ### Rendering example (focused, value = "CPGen")
 * @code
 * ╭─── Project Name ────────────╮
 * │ CPGen                       │
 * ╰─────────────────────────────╯
 * @endcode
 */
class Input : public Component {
public:
  /**
   * @brief Construct an Input with a label and a change callback.
   * @param label     Descriptive text shown in the top border of the widget.
   * @param on_change Callback invoked with the current full value string after
   *                  every character is added or removed.
   */
  Input(std::string label, std::function<void(std::string)> on_change);

  /**
   * @brief Render the input box as a multi-line ANSI string.
   * @return Three-line string (top border, content, bottom border).
   */
  [[nodiscard]] std::string render() const override;

  /**
   * @brief Handle a key event.
   *
   * - Printable @c char → appended to the current value.
   * - @ref Defs::Special::Backspace → removes the last character.
   * - All other keys → ignored (returns @c false).
   *
   * @param key The key to process.
   * @return @c true if the key was consumed, @c false otherwise.
   */
  bool handleInput(Defs::Key key) override;

private:
  std::string m_label; ///< Label displayed in the top border.
  std::string m_value; ///< Current text entered by the user.
  std::function<void(std::string)>
      m_on_change; ///< Callback fired after every edit.
};

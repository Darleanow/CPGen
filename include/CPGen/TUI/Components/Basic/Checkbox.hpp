#pragma once

/**
 * @file Checkbox.hpp
 * @brief A binary toggle widget that displays a checked/unchecked state.
 * @author Darleanow
 */

#include "Components/Component.hpp"
#include <functional>
#include <string>

/**
 * @class Checkbox
 * @brief A toggleable checkbox component.
 *
 * Renders a Unicode checkbox glyph (checked or unchecked) followed by a text
 * label.  When the component is focused, an arrow cursor is prepended to
 * indicate selection.  Pressing @ref Defs::Special::Enter toggles the state and
 * fires the @c on_change callback.
 *
 * ### Rendering example (focused, unchecked)
 * @code
 *   ➡  ☐  Enable git ?
 * @endcode
 *
 * ### Rendering example (unfocused, checked)
 * @code
 *      ✔  Enable git ?
 * @endcode
 */
class Checkbox : public Component {
public:
  /**
   * @brief Construct a Checkbox with a label and a change callback.
   * @param label     Descriptive text displayed next to the checkbox glyph.
   * @param on_change Callback invoked with the new checked state whenever the
   *                  checkbox is toggled.
   */
  Checkbox(std::string label, std::function<void(bool)> on_change);

  /**
   * @brief Return the current checked state.
   * @return @c true if the checkbox is checked.
   */
  [[nodiscard]] bool isChecked() const;

  /**
   * @brief Render the checkbox as an ANSI string.
   * @return A single-line string containing the cursor (if focused), the
   *         checkbox glyph, and the label.
   */
  [[nodiscard]] std::string render() const override;

  /**
   * @brief Handle a key event.
   *
   * Toggles the checked state when @ref Defs::Special::Enter is pressed.
   * All other keys are ignored (returns @c false).
   *
   * @param key The key to process.
   * @return @c true if @c Enter was consumed, @c false otherwise.
   */
  bool handleInput(Defs::Key key) override;

private:
  /**
   * @brief Flip the checked state and invoke the @c on_change callback.
   */
  void toggle();

  std::string m_label;                        ///< Display label shown next to the glyph.
  bool        m_is_checked = false;           ///< Current toggle state.
  std::function<void(bool)> m_on_change;      ///< Callback fired on every toggle.
};

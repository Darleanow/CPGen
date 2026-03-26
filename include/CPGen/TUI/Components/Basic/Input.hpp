#pragma once

#include "CPGen/TUI/Components/Component.hpp"
#include <functional>
#include <string>

/**
 * @class Input
 * @brief A single-line text input component
 *
 * Renders a bordered box with a label and editable value.
 * Handles character input, backspace deletion, and fires
 * on_change(string) callback on each modification.
 */
class Input : public Component {
public:
  Input(std::string label, std::function<void(std::string)> on_change);

  std::string render() const override;
  bool handleInput(Defs::Key key) override;

private:
  std::string m_label;
  std::string m_value;

  std::function<void(std::string)> m_on_change;
};

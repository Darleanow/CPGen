#pragma once

#include "CPGen/TUI/Components/Component.hpp"
#include <functional>
#include <string>

/**
 * @class Checkbox
 * @brief A toggleable checkbox component
 *
 * Displays a label with a checked/unchecked icon.
 * Fires on_change(bool) callback when toggled via Enter.
 */
class Checkbox : public Component {
public:
  Checkbox(std::string label, std::function<void(bool)> on_change);

  bool isChecked() const;

  std::string render() const override;
  bool handleInput(Defs::Key key) override;

private:
  std::string m_label;
  bool m_is_checked = false;

  std::function<void(bool)> m_on_change;

  void toggle();
};

#include "Components/Basic/Checkbox.hpp"
#include "Misc/Ascii.hpp"
#include "Misc/Defs.hpp"
#include "Misc/Utils.hpp"
#include <functional>
#include <string>
#include <utility>
#include <variant>

Checkbox::Checkbox(std::string label, std::function<void(bool)> on_change)
    : m_label(std::move(label)), m_on_change(std::move(on_change)) {}

void Checkbox::toggle() {
  m_is_checked = !m_is_checked;
  m_on_change(m_is_checked);
}

bool Checkbox::isChecked() const { return m_is_checked; }

std::string Checkbox::render() const {
  std::string out;

  if (m_is_focused) {
    out += Utils::Colours::esc(Utils::Colours::BG_WHITE);
    out += Utils::Colours::esc(Utils::Colours::FG_BLACK);
    out += Utils::Colours::esc(Utils::Colours::BOLD);
  }

  out += (m_is_checked ? Ascii::CHECKBOX_CHECKED : Ascii::CHECKBOX_UNCHECKED);
  out += " " + m_label;
  out += Utils::Colours::esc(Utils::Colours::RESET);

  return out;
}

bool Checkbox::handleInput(Defs::Key key) {
  if (std::holds_alternative<Defs::Special>(key)) {
    if (std::get<Defs::Special>(key) == Defs::Special::Enter) {
      toggle();
      return true;
    }
  }
  return false;
}

#include "CPGen/TUI/Components/Basic/Checkbox.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/Misc/Utils.hpp"

Checkbox::Checkbox(std::string label, std::function<void(bool)> on_change)
    : m_label(std::move(label)), m_on_change(std::move(on_change)) {}

void Checkbox::toggle() {
  m_is_checked = !m_is_checked;
  m_on_change(m_is_checked);
}

bool Checkbox::isChecked() const { return m_is_checked; }

std::string Checkbox::render() const {
  using namespace Utils;
  std::string out;

  if (m_is_focused) {
    out += Colours::esc(Colours::BG_WHITE);
    out += Colours::esc(Colours::FG_BLACK);
    out += Colours::esc(Colours::BOLD);
  }

  out += (m_is_checked ? Ascii::CheckboxChecked : Ascii::CheckboxUnchecked);
  out += " " + m_label;
  out += Colours::esc(Colours::RESET);

  return out;
}

bool Checkbox::handleInput(Defs::Key key) {
  return std::visit(
      [this](auto k) -> bool {
        using T = decltype(k);
        if constexpr (std::is_same_v<T, Defs::Special>) {
          if (k == Defs::Special::Enter) {
            toggle();
            return true;
          }
        }
        return false;
      },
      key);
}

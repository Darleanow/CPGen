#include "CPGen/TUI/Components/Basic/Checkbox.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"

Checkbox::Checkbox(
    std::string value_name,
    std::function<void(bool)> callback) // I prefix here with space, simpler
    : m_value_name(" " + std::move(value_name)), m_is_active(false),
      m_callback(callback) {}

void Checkbox::check() {
  m_is_active = !m_is_active;
  m_callback(m_is_active);
}

bool Checkbox::isActive() const { return m_is_active; }

std::string Checkbox::render() const {
  return (m_is_active ? Ascii::CheckboxChecked : Ascii::CheckboxUnchecked) +
         m_value_name;
}

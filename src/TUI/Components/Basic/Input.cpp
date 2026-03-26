#include "CPGen/TUI/Components/Basic/Input.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/Misc/Utils.hpp"

Input::Input(std::string label, std::function<void(std::string)> on_change)
    : m_label(std::move(label)), m_on_change(std::move(on_change)) {}

std::string Input::render() const {
  using namespace Utils;

  constexpr size_t width = 30;
  std::string out;

  // Title colour when focused
  if (m_is_focused) {
    out += Colours::esc(Colours::FG_CYAN);
    out += Colours::esc(Colours::BOLD);
  } else {
    out += Colours::esc(Colours::DIM);
  }

  // Top border
  size_t label_pad = (m_label.size() < width) ? width - m_label.size() : 0;
  out += Ascii::BoxCornerTopLeft + m_label;
  for (size_t i = 0; i < label_pad; i++) {
    out += Ascii::BoxHorizontal;
  }
  out += std::string(Ascii::BoxCornerTopRight) + Colours::esc(Colours::RESET) +
         "\n";

  // Content line — show the tail of the value if it overflows
  if (m_is_focused) {
    out += Colours::esc(Colours::FG_CYAN);
  } else {
    out += Colours::esc(Colours::DIM);
  }
  out += Ascii::BoxVertical;
  out += Colours::esc(Colours::RESET);

  size_t content_width = m_is_focused ? width - 1 : width;

  std::string visible = m_value;
  if (visible.size() > content_width) {
    visible = visible.substr(visible.size() - content_width);
  }
  out += visible;

  // Cursor
  if (m_is_focused) {
    out += Colours::esc(Colours::FG_BWHITE);
    out += Colours::esc(Colours::BLINK);
    out += "\u2587";
    out += Colours::esc(Colours::RESET);
  }

  for (size_t i = 0; i < content_width - visible.size(); i++) {
    out += " ";
  }

  if (m_is_focused) {
    out += Colours::esc(Colours::FG_CYAN);
  } else {
    out += Colours::esc(Colours::DIM);
  }
  out += std::string(Ascii::BoxVertical) + Colours::esc(Colours::RESET) + "\n";

  // Bottom border
  if (m_is_focused) {
    out += Colours::esc(Colours::FG_CYAN);
  } else {
    out += Colours::esc(Colours::DIM);
  }
  out += Ascii::BoxCornerBottomLeft;
  for (size_t i = 0; i < width; i++) {
    out += Ascii::BoxHorizontal;
  }
  out += std::string(Ascii::BoxCornerBottomRight) +
         Colours::esc(Colours::RESET) + "\n";

  return out;
}

bool Input::handleInput(Defs::Key key) {
  return std::visit(
      [this](auto k) -> bool {
        using T = decltype(k);
        if constexpr (std::is_same_v<T, char>) {
          if (k >= 32 && k < 127) { // Printable ASCII
            m_value += k;
            m_on_change(m_value);
            return true;
          }
          return false;
        } else if constexpr (std::is_same_v<T, Defs::Special>) {
          if (k == Defs::Special::Backspace && !m_value.empty()) {
            m_value.pop_back();
            m_on_change(m_value);
            return true;
          }
        }
        return false;
      },
      key);
}

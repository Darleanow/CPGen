#include "Components/Basic/Input.hpp"
#include "Misc/Ascii.hpp"
#include "Misc/Defs.hpp"
#include "Misc/Utils.hpp"
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <variant>

Input::Input(std::string label, std::function<void(std::string)> on_change)
    : m_label(std::move(label)), m_on_change(std::move(on_change)) {}

std::string Input::render() const {
  constexpr size_t WIDTH = 30;

  const std::string clr = m_is_focused
                              ? Utils::Colours::esc(Utils::Colours::FG_CYAN)
                              : Utils::Colours::esc(Utils::Colours::DIM);

  std::string out;
  out += clr;
  if (m_is_focused) {
    out += Utils::Colours::esc(Utils::Colours::BOLD);
  }

  const size_t label_pad =
      (m_label.size() < WIDTH) ? WIDTH - m_label.size() : 0;
  out += Ascii::BOX_CORNER_TOP_LEFT + m_label;
  for (size_t i = 0; i < label_pad; i++) {
    out += Ascii::BOX_HORIZONTAL;
  }
  out += std::string(Ascii::BOX_CORNER_TOP_RIGHT) +
         Utils::Colours::esc(Utils::Colours::RESET) + "\n";

  out += clr;
  out += Ascii::BOX_VERTICAL;
  out += Utils::Colours::esc(Utils::Colours::RESET);

  const size_t content_width = m_is_focused ? WIDTH - 1 : WIDTH;

  std::string visible = m_value;
  if (visible.size() > content_width) {
    visible = visible.substr(visible.size() - content_width);
  }
  out += visible;

  if (m_is_focused) {
    out += Utils::Colours::esc(Utils::Colours::FG_BWHITE);
    out += Utils::Colours::esc(Utils::Colours::BLINK);
    out += "\u2587";
    out += Utils::Colours::esc(Utils::Colours::RESET);
  }

  for (size_t i = 0; i < content_width - visible.size(); i++) {
    out += " ";
  }

  out += clr;
  out += std::string(Ascii::BOX_VERTICAL) +
         Utils::Colours::esc(Utils::Colours::RESET) + "\n";

  out += clr;
  out += Ascii::BOX_CORNER_BOTTOM_LEFT;
  for (size_t i = 0; i < WIDTH; i++) {
    out += Ascii::BOX_HORIZONTAL;
  }
  out += std::string(Ascii::BOX_CORNER_BOTTOM_RIGHT) +
         Utils::Colours::esc(Utils::Colours::RESET) + "\n";

  return out;
}

bool Input::handleInput(Defs::Key key) {
  if (std::holds_alternative<char>(key)) {
    const char k = std::get<char>(key);
    if (k >= 32 && k < 127) {
      m_value += k;
      m_on_change(m_value);
      return true;
    }
    return false;
  }
  if (std::holds_alternative<Defs::Special>(key)) {
    if (std::get<Defs::Special>(key) == Defs::Special::Backspace &&
        !m_value.empty()) {
      m_value.pop_back();
      m_on_change(m_value);
      return true;
    }
  }
  return false;
}

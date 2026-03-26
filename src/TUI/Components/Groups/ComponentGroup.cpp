#include "CPGen/TUI/Components/Groups/ComponentGroup.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include "CPGen/TUI/Misc/Utils.hpp"
#include <stdexcept>

ComponentGroup::ComponentGroup(std::string title, std::string icon)
    : m_title(std::move(title)), m_icon(std::move(icon)) {}

void ComponentGroup::addChild(std::unique_ptr<Component> child) {
  m_children.push_back(std::move(child));
}

void ComponentGroup::setFocused(bool focused) {
  m_is_focused = focused;

  if (m_children.empty())
    return;

  if (focused) {
    m_children.at(m_focused_index)->setFocused(true);
  } else {
    for (auto &child : m_children) {
      child->setFocused(false);
    }
  }
}

bool ComponentGroup::moveFocus(int dir) {
  if (m_children.empty())
    return false;

  auto next = static_cast<int>(m_focused_index) + dir;

  if (next < 0 || next >= static_cast<int>(m_children.size()))
    return false;

  m_children.at(m_focused_index)->setFocused(false);
  m_focused_index = static_cast<size_t>(next);
  m_children.at(m_focused_index)->setFocused(true);

  return true;
}

std::string ComponentGroup::render() const {
  using namespace Utils;

  constexpr size_t width = 30;
  std::string out;

  // Header colour
  if (m_is_focused) {
    out += Colours::esc(Colours::FG_CYAN);
    out += Colours::esc(Colours::BOLD);
  } else {
    out += Colours::esc(Colours::DIM);
  }

  // Top border with title
  out += Ascii::BoxCornerTopLeft;
  out += Ascii::BoxHorizontal;
  if (!m_icon.empty()) {
    out += m_icon + " ";
  }
  out += m_title;
  out += Ascii::BoxHorizontal;
  for (size_t i = 0; i < width - (m_title.size() +
                                  std::string(Ascii::BoxHorizontal).size() + 1);
       i++) {
    out += Ascii::BoxHorizontal;
  }
  out += Ascii::BoxCornerTopRight;
  out += Colours::esc(Colours::RESET) + "\n";

  // Children
  for (const auto &child : m_children) {
    if (m_is_focused) {
      out += Colours::esc(Colours::FG_CYAN);
    } else {
      out += Colours::esc(Colours::DIM);
    }
    out += Ascii::BoxVertical;
    out += Colours::esc(Colours::RESET);
    auto render_str = child->render();
    out += render_str;
    auto vis_w = Utils::visualWidth(render_str);
    if (vis_w < width) {
      for (size_t i = 0; i < width - vis_w; i++) {
        out += ' ';
      }
    }
    if (m_is_focused) {
      out += Colours::esc(Colours::FG_CYAN);
    } else {
      out += Colours::esc(Colours::DIM);
    }
    out += Ascii::BoxVertical;
    out += Colours::esc(Colours::RESET);
    out += "\n";
  }

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
  out += Ascii::BoxCornerBottomRight;
  out += Colours::esc(Colours::RESET) + "\n";

  return out;
}

bool ComponentGroup::handleInput(Defs::Key key) {
  if (m_children.empty())
    return false;

  // First let the focused child try to handle it
  if (m_children.at(m_focused_index)->handleInput(key))
    return true;

  // Child didn't consume it — try navigation
  return std::visit(
      [this](auto k) -> bool {
        using T = decltype(k);
        if constexpr (std::is_same_v<T, Defs::Special>) {
          if (k == Defs::Special::Down)
            return moveFocus(1);
          if (k == Defs::Special::Up)
            return moveFocus(-1);
        }
        return false;
      },
      key);
}

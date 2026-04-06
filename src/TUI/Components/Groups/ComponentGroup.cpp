#include "Components/Groups/ComponentGroup.hpp"
#include "Components/Component.hpp"
#include "Misc/Ascii.hpp"
#include "Misc/Defs.hpp"
#include "Misc/Utils.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <variant>

ComponentGroup::ComponentGroup(std::string title, std::string icon)
    : m_title(std::move(title)), m_icon(std::move(icon)) {}

void ComponentGroup::addChild(std::unique_ptr<Component> child) {
  m_children.push_back(std::move(child));
}

void ComponentGroup::setFocused(bool focused) {
  m_is_focused = focused;

  if (m_children.empty()) {
    return;
  }

  if (focused) {
    m_children.at(m_focused_index)->setFocused(/*focused=*/true);
  } else {
    for (auto &child : m_children) {
      child->setFocused(/*focused=*/false);
    }
  }
}

bool ComponentGroup::moveFocus(int dir) {
  if (m_children.empty()) {
    return false;
  }

  auto next = static_cast<int>(m_focused_index) + dir;

  if (next < 0 || next >= static_cast<int>(m_children.size())) {
    return false;
  }

  m_children.at(m_focused_index)->setFocused(/*focused=*/false);
  m_focused_index = static_cast<size_t>(next);
  m_children.at(m_focused_index)->setFocused(/*focused=*/true);

  return true;
}

std::string ComponentGroup::render() const {
  constexpr size_t WIDTH = 30;

  const std::string clr = m_is_focused
                              ? Utils::Colours::esc(Utils::Colours::FG_CYAN)
                              : Utils::Colours::esc(Utils::Colours::DIM);

  std::string out;
  out += clr;
  if (m_is_focused) {
    out += Utils::Colours::esc(Utils::Colours::BOLD);
  }

  out += Ascii::BOX_CORNER_TOP_LEFT;
  out += Ascii::BOX_HORIZONTAL;
  if (!m_icon.empty()) {
    out += m_icon + " ";
  }
  out += m_title;
  out += Ascii::BOX_HORIZONTAL;
  for (size_t i = 0;
       i < WIDTH - (m_title.size() +
                    std::string(Ascii::BOX_HORIZONTAL).size() + 1);
       i++) {
    out += Ascii::BOX_HORIZONTAL;
  }
  out += Ascii::BOX_CORNER_TOP_RIGHT;
  out += Utils::Colours::esc(Utils::Colours::RESET) + "\n";

  for (const auto &child : m_children) {
    out += clr;
    out += Ascii::BOX_VERTICAL;
    out += Utils::Colours::esc(Utils::Colours::RESET);
    auto render_str = child->render();
    out += render_str;
    const auto vis_w = Utils::visualWidth(render_str);
    if (vis_w < WIDTH) {
      out += std::string(WIDTH - vis_w, ' ');
    }
    out += clr;
    out += Ascii::BOX_VERTICAL;
    out += Utils::Colours::esc(Utils::Colours::RESET);
    out += "\n";
  }

  out += clr;
  out += Ascii::BOX_CORNER_BOTTOM_LEFT;
  for (size_t i = 0; i < WIDTH; i++) {
    out += Ascii::BOX_HORIZONTAL;
  }
  out += Ascii::BOX_CORNER_BOTTOM_RIGHT;
  out += Utils::Colours::esc(Utils::Colours::RESET) + "\n";

  return out;
}

bool ComponentGroup::handleInput(Defs::Key key) {
  if (m_children.empty()) {
    return false;
  }
  if (m_children.at(m_focused_index)->handleInput(key)) {
    return true;
  }
  if (!std::holds_alternative<Defs::Special>(key)) {
    return false;
  }
  const auto special = std::get<Defs::Special>(key);
  if (special == Defs::Special::Down) {
    return moveFocus(/*dir=*/1);
  }
  if (special == Defs::Special::Up) {
    return moveFocus(/*dir=*/-1);
  }
  return false;
}

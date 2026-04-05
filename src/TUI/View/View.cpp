#include "View/View.hpp"
#include "Misc/Terminal.hpp"
#include "Misc/Utils.hpp"
#include <iostream>

void View::addSection(std::unique_ptr<Component> section) {
  m_sections.push_back(std::move(section));
}

void View::redraw() const {
  // Clear screen and move cursor to top-left
  std::cout << "\033[2J\033[H";

  for (const auto &section : m_sections) {
    std::cout << section->render() << "\n";
  }

  std::cout.flush();
}

void View::moveFocus(int dir) {
  auto next = static_cast<int>(m_focused_index) + dir;

  if (next < 0 || next >= static_cast<int>(m_sections.size()))
    return;

  m_sections.at(m_focused_index)->setFocused(false);
  m_focused_index = static_cast<size_t>(next);
  m_sections.at(m_focused_index)->setFocused(true);
}

void View::run() {
  if (m_sections.empty())
    return;

  Terminal term; // RAII: raw mode ON

  m_running = true;
  m_focused_index = 0;
  m_sections.at(0)->setFocused(true);

  while (m_running) {
    redraw();

    auto key = Utils::readKey();

    // Escape quits
    if (auto *special = std::get_if<Defs::Special>(&key)) {
      if (*special == Defs::Special::Escape) {
        m_running = false;
        break;
      }
    }

    bool consumed = m_sections.at(m_focused_index)->handleInput(key);

    // If not consumed, try top-level navigation
    if (!consumed) {
      std::visit(
          [this](auto k) {
            using T = decltype(k);
            if constexpr (std::is_same_v<T, Defs::Special>) {
              if (k == Defs::Special::Down)
                moveFocus(1);
              else if (k == Defs::Special::Up)
                moveFocus(-1);
            }
          },
          key);
    }
  }

  redraw();
}

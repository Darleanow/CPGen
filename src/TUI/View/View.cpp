#include "View/View.hpp"
#include "Components/Component.hpp"
#include "Misc/Defs.hpp"
#include "Misc/Terminal.hpp"
#include "Misc/Utils.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>
#include <variant>

void View::addSection(std::unique_ptr<Component> section) {
  m_sections.push_back(std::move(section));
}

void View::redraw() const {
  std::cout << "\033[2J\033[H";

  for (const auto &section : m_sections) {
    std::cout << section->render() << "\n";
  }

  std::cout.flush();
}

void View::moveFocus(int dir) {
  auto next = static_cast<int>(m_focused_index) + dir;

  if (next < 0 || next >= static_cast<int>(m_sections.size())) {
    return;
  }

  m_sections.at(m_focused_index)->setFocused(/*focused=*/false);
  m_focused_index = static_cast<size_t>(next);
  m_sections.at(m_focused_index)->setFocused(/*focused=*/true);
}

void View::run() {
  if (m_sections.empty()) {
    return;
  }

  const Terminal term;

  m_running = true;
  m_focused_index = 0;
  m_sections.at(0)->setFocused(/*focused=*/true);

  while (m_running) {
    redraw();

    auto key = Utils::readKey();

    if (std::holds_alternative<Defs::Special>(key) &&
        std::get<Defs::Special>(key) == Defs::Special::Escape) {
      break;
    }

    const bool consumed = m_sections.at(m_focused_index)->handleInput(key);
    if (consumed || !std::holds_alternative<Defs::Special>(key)) {
      continue;
    }

    const auto special = std::get<Defs::Special>(key);
    if (special == Defs::Special::Down) {
      moveFocus(/*dir=*/1);
    }
    if (special == Defs::Special::Up) {
      moveFocus(/*dir=*/-1);
    }
  }

  redraw();
}

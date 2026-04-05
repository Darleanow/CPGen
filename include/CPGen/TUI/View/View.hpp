#pragma once

#include "Components/Component.hpp"
#include <memory>
#include <vector>

/**
 * @class View
 * @brief Top-level TUI view that manages sections and the main loop
 *
 * Holds a flat list of top-level components (sections),
 * manages focus between them, and runs the input loop.
 * Escape exits the loop and restores the terminal.
 */
class View {
public:
  void addSection(std::unique_ptr<Component> section);

  void run();

private:
  std::vector<std::unique_ptr<Component>> m_sections;
  size_t m_focused_index = 0;
  bool m_running = false;

  void redraw() const;
  void moveFocus(int dir);
};

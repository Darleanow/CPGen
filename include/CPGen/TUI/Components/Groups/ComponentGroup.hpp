#pragma once

#include "CPGen/TUI/Components/Component.hpp"
#include <memory>
#include <string>
#include <vector>

/**
 * @class ComponentGroup
 * @brief A generic container that holds child components
 *
 * Renders a titled box with an optional icon, containing
 * any number of child Components. Manages internal focus
 * navigation (Up/Down) and delegates input to the focused child.
 */
class ComponentGroup : public Component {
public:
  ComponentGroup(std::string title, std::string icon = "");

  void addChild(std::unique_ptr<Component> child);

  std::string render() const override;
  bool handleInput(Defs::Key key) override;

  void setFocused(bool focused) override;

private:
  std::string m_title;
  std::string m_icon;
  std::vector<std::unique_ptr<Component>> m_children;
  size_t m_focused_index = 0;

  bool moveFocus(int dir);
};

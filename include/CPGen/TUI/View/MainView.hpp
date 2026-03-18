#pragma once

#include "CPGen/TUI/Components/Component.hpp"
#include <memory>
#include <vector>

class MainView {
public:
  void addComponent(std::unique_ptr<Component> component);

  void render();

private:
  std::vector<std::unique_ptr<Component>> m_components;
};

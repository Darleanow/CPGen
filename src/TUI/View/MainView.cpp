#include "CPGen/TUI/View/MainView.hpp"
#include <iostream>

void MainView::addComponent(std::unique_ptr<Component> component) {
  m_components.push_back(std::move(component));
}

void MainView::render() {
  for (auto &component : m_components) {
    std::cout << component->render();
  }
}

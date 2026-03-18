#pragma once
#include "CPGen/TUI/Components/Basic/Checkbox.hpp"
#include "CPGen/TUI/Components/Component.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class CheckboxGroup : public Component {
public:
  CheckboxGroup(std::string group_name, std::vector<std::string> checkboxes,
                std::vector<std::function<void(bool)>> callbacks,
                std::string icon = "");

  std::string render() const override;

private:
  std::string m_group_name;
  std::vector<std::unique_ptr<Checkbox>> m_checkbox_group;
  std::string m_icon;

  size_t m_current_selection;
};

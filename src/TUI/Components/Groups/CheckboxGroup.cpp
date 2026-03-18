#include "CPGen/TUI/Components/Groups/CheckboxGroup.hpp"
#include "CPGen/TUI/Misc/Ascii.hpp"
#include <stdexcept>

CheckboxGroup::CheckboxGroup(std::string group_name,
                             std::vector<std::string> checkboxes,
                             std::vector<std::function<void(bool)>> callbacks,
                             std::string icon)
    : m_group_name(std::move(group_name)), m_icon(std::move(icon)),
      m_current_selection(0) {

  if (checkboxes.size() <= 0 || callbacks.size() <= 0) {
    throw std::runtime_error("Vectors for checkbox group can't be empty !");
  }

  if (checkboxes.size() != callbacks.size()) {
    throw std::runtime_error("Mismatch in checkboxes and callbacks count");
  }

  for (size_t i = 0; i < checkboxes.size(); i++) {
    auto checkbox =
        std::make_unique<Checkbox>(checkboxes.at(i), callbacks.at(i));
    m_checkbox_group.push_back(std::move(checkbox));
  }

  if (m_checkbox_group.size() == 0) {
    throw std::runtime_error("Can't initialize an empty checkbox group !");
  }
}

std::string CheckboxGroup::render() const {
  std::string render;
  size_t total_width = m_group_name.size() + 2; // Name length + 2 for the sides

  render += std::string(Ascii::BoxCornerTopLeft) + Ascii::BoxHorizontal +
            m_group_name + Ascii::BoxHorizontal + "\n";

  for (const auto &checkbox : m_checkbox_group) {
    render += Ascii::BoxVertical + checkbox->render() + "\n";
  }
  render += Ascii::BoxCornerBottomLeft;
  for (size_t i = 0; i < total_width; i++) {
    render += Ascii::BoxHorizontal;
  }
  render += "\n";

  return render;
}

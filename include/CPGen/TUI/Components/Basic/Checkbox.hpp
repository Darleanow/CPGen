#pragma once

#include "CPGen/TUI/Components/Component.hpp"
#include <functional>
#include <string>

class Checkbox : Component {
public:
  Checkbox(std::string value_name, std::function<void(bool)> callback);

  void check();
  bool isActive() const;

  std::string render() const override;

private:
  std::string m_value_name;
  bool m_is_active;

  std::function<void(bool)> m_callback;
};

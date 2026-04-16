#pragma once

#include "CPGen/Core/Resolved.hpp"

class CMakeGenerator {
public:
  [[nodiscard]] static std::string
  generate(const ResolvedProject &resolved_project);
};

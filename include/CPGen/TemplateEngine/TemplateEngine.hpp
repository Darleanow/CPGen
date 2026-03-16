#pragma once

/**
 * @file TemplateEngine.hpp
 * @brief Replaces placeholders by actual values
 * @author Darleanow
 * @version 1.0.0
 */

#include <string>
#include <unordered_map>

/**
 * @class TemplateEngine
 */
class TemplateEngine {
public:
  /**
   * @brief Renders a file content into a properly formatted string
   * @param content: The file content with placeholders
   * @param opts: The KV map for reference
   * @return The new file content updated
   */
  static std::string render(std::string &content,
                            std::unordered_map<std::string, std::string> opts);
};

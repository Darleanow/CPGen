#include "CPGen/TemplateEngine/TemplateEngine.hpp"

std::string
TemplateEngine::render(std::string &content,
                       std::unordered_map<std::string, std::string> opts) {
  size_t pos = 0;

  while ((pos = content.find("{{", pos)) != std::string::npos) {
    size_t end = content.find("}}", pos);
    if (end == std::string::npos) {
      break;
    }

    std::string key = content.substr(pos + 2, end - pos - 2);

    auto it = opts.find(key);
    std::string value = (it != opts.end()) ? it->second : "";

    content.replace(pos, end + 2 - pos, value);

    pos += value.size();
  }

  return content;
}

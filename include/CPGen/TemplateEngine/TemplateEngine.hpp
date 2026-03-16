#pragma once
#include <string>
#include <unordered_map>

class TemplateEngine {
public:
  static std::string render(std::string &content,
                            std::unordered_map<std::string, std::string> opts);

private:
};

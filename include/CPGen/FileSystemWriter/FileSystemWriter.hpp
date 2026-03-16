#pragma once
#include <filesystem>

class FileSystemWriter {
public:
  std::string readFile(const std::filesystem::path &path);
  void writeFile(const std::filesystem::path &path, const std::string &content);
};

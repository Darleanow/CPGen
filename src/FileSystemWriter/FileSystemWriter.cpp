#include "CPGen/FileSystemWriter/FileSystemWriter.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

std::string FileSystemWriter::readFile(const std::filesystem::path &path) {
  std::ifstream file(path);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + path.string());
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void FileSystemWriter::writeFile(const std::filesystem::path &path,
                                 const std::string &content) {
  std::ofstream file(path);
  file << content;
}

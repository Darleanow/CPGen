#pragma once

/**
 * @file FileSystemWriter.hpp
 * @brief FS operations
 * @author Darleanow
 * @version 1.0.0
 */

#include <filesystem>

/**
 * @class FileSystemWriter
 * @brief Permits reading and writing file based on our use case
 */
class FileSystemWriter {
public:
  /**
   * @brief Reads a file and returns a string of its content
   * @param path: A path to a file
   * @return A string that has the file contents
   */
  std::string readFile(const std::filesystem::path &path);

  /**
   * @brief Writes to a file
   * @param path: Path to write at
   * @param content: What to actually write
   */
  void writeFile(const std::filesystem::path &path, const std::string &content);
};

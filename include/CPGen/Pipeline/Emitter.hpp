#pragma once

/**
 * @file Emitter.hpp
 * @brief Base interface for all pipeline emitters.
 * @author Darleanow
 *
 * An emitter transforms a @ref ResolvedProject into a set of output files.
 * The @ref GenerationPipeline collects outputs from all registered emitters
 * and writes them to disk via @c std::filesystem.
 */

#include "CPGen/Core/Resolved.hpp"

#include <filesystem>
#include <string>
#include <vector>

/**
 * @struct OutputFile
 * @brief Represents a single file to be written during project generation.
 */
struct OutputFile {
  std::filesystem::path relative_path; ///< Path relative to the project root.
  std::string content;                 ///< Full file content.
};

/**
 * @class Emitter
 * @brief Abstract base class for pipeline emitters.
 *
 * Each concrete emitter is responsible for one facet of project generation
 * (CMake build file, source scaffolding, tooling configuration, …).
 */
class Emitter {
public:
  Emitter() = default;
  virtual ~Emitter() = default;
  Emitter(const Emitter &) = default;
  Emitter &operator=(const Emitter &) = default;
  Emitter(Emitter &&) = default;
  Emitter &operator=(Emitter &&) = default;

  /**
   * @brief Produce output files from a resolved project.
   * @param project The fully resolved project description.
   * @return A vector of files to write to the project directory.
   */
  [[nodiscard]] virtual std::vector<OutputFile>
  emit(const ResolvedProject &project) = 0;
};

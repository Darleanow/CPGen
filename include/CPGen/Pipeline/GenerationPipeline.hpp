#pragma once

/**
 * @file GenerationPipeline.hpp
 * @brief Orchestrates emitters and writes the generated project to disk.
 * @author Darleanow
 *
 * The pipeline collects @ref OutputFile entries from all registered @ref
 * Emitter instances and writes them to the target directory using
 * @c std::filesystem / @c std::ofstream — no custom FileWriter abstraction.
 */

#include "CPGen/Core/Resolved.hpp"
#include "CPGen/Pipeline/Emitter.hpp"

#include <filesystem>
#include <memory>
#include <vector>

/**
 * @class GenerationPipeline
 * @brief Runs all registered emitters and materialises the project on disk.
 *
 * Typical usage:
 * @code
 *   GenerationPipeline pipeline;
 *   pipeline.addEmitter(std::make_unique<CMakeEmitter>());
 *   pipeline.addEmitter(std::make_unique<ScaffoldEmitter>(template_root));
 *   pipeline.addEmitter(std::make_unique<ToolingEmitter>(template_root));
 *   pipeline.execute(resolved_project, "/tmp/MyProject");
 * @endcode
 */
class GenerationPipeline {
public:
  /**
   * @brief Register an emitter to be run during @ref execute.
   * @param emitter Owning pointer to the emitter.
   */
  void addEmitter(std::unique_ptr<Emitter> emitter);

  /**
   * @brief Run every registered emitter and write the resulting files.
   *
   * Creates the @p output_dir (and any parent directories) if they do not
   * exist, then iterates through all emitters, collects their output, and
   * writes each file.
   *
   * @param project    The resolved project to generate.
   * @param output_dir Absolute path to the root directory of the new project.
   * @throws std::runtime_error If a file cannot be written.
   */
  void execute(const ResolvedProject &project,
               const std::filesystem::path &output_dir);

private:
  std::vector<std::unique_ptr<Emitter>> m_emitters;
};

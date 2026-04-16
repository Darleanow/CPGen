/**
 * @file GenerationPipeline.cpp
 * @brief Implements the generation pipeline orchestrator.
 *
 * Runs all registered @ref Emitter instances against a @ref ResolvedProject,
 * collects the @ref OutputFile results, and writes them to the target
 * directory using @c std::filesystem and @c std::ofstream.
 *
 * @author Darleanow
 */

#include "CPGen/Pipeline/GenerationPipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <utility>

void GenerationPipeline::addEmitter(std::unique_ptr<Emitter> emitter) {
  m_emitters.push_back(std::move(emitter));
}

void GenerationPipeline::execute(
    const ResolvedProject &project,
    const std::filesystem::path &output_dir) {

  std::filesystem::create_directories(output_dir);

  for (const auto &emitter : m_emitters) {
    for (const auto &file : emitter->emit(project)) {
      const auto full_path = output_dir / file.relative_path;

      std::filesystem::create_directories(full_path.parent_path());

      std::ofstream out(full_path, std::ios::trunc);
      if (!out.is_open()) {
        throw std::runtime_error("Failed to write: " + full_path.string());
      }
      out << file.content;
    }
  }
}

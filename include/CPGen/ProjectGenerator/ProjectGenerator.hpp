#pragma once

/**
 * @file ProjectGenerator.hpp
 * @brief Orchestrator of the write ops and template engine
 * @author Darleanow
 * @version 1.0.0
 */

#include "CPGen/CLI/CLI.hpp"
#include "CPGen/FileSystemWriter/FileSystemWriter.hpp"
#include "CPGen/TemplateEngine/TemplateEngine.hpp"

/**
 * @class ProjectGenerator
 * @brief Generates a project by using FileSystemWriter and TemplateEngine
 */
class ProjectGenerator {
public:
  /**
   * @brief Constructor
   * Stores cli options
   * @param opts: The cli options, parsed
   */
  ProjectGenerator(const CLIOpts &opts);

  /**
   * @brief Generates the project
   */
  void generateProject();

private:
  TemplateEngine m_template_engine; /**< Template Engine, handling the
                                       replacement of the placeholders */
  FileSystemWriter m_writer;        /**< Writer, writes to files */
  const CLIOpts &m_opts;            /**< CLIOpts, the cli options */
};

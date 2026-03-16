#pragma once

#include "CPGen/CLI/CLI.hpp"
#include "CPGen/FileSystemWriter/FileSystemWriter.hpp"
#include "CPGen/TemplateEngine/TemplateEngine.hpp"

class ProjectGenerator {
public:
  ProjectGenerator(const CLIOpts &opts);

  void generate_project();

private:
  TemplateEngine m_template_engine;
  FileSystemWriter m_writer;
  const CLIOpts &m_opts;
};

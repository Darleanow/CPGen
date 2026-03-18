#include "CPGen/ProjectGenerator/ProjectGenerator.hpp"

ProjectGenerator::ProjectGenerator(const CLIOpts &opts) : m_opts(opts) {
  m_template_engine = TemplateEngine();
  m_writer = FileSystemWriter();
}

void ProjectGenerator::generateProject() {
  namespace fs = std::filesystem;

  fs::path source = "./templates/base";
  fs::path dest = fs::path(m_opts.path) / m_opts.name;

  auto umap_opts = CLI::parse_opts(m_opts);

  fs::create_directories(dest);

  for (const auto &entry : fs::recursive_directory_iterator(source)) {
    fs::path rel = fs::relative(entry.path(), source);
    fs::path target = dest / rel;

    if (entry.is_directory()) {
      fs::create_directories(target);
    } else {
      fs::create_directories(target.parent_path());
      auto content = m_writer.readFile(entry.path());
      const auto &rendered_content =
          m_template_engine.render(content, umap_opts);
      m_writer.writeFile(target, rendered_content);
    }
  }
}

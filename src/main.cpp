#include "CPGen/CLI/CLI.hpp"
#include "CPGen/ProjectGenerator/ProjectGenerator.hpp"

int main(int argc, char **argv) {
  CLI cli;
  ProjectGenerator generator(cli.parse(argc, argv));

  generator.generate_project();

  return 0;
}

cmake_minimum_required(VERSION 4.0)
project(
  CPGen
  VERSION 1.0.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v6.1.9)

FetchContent_MakeAvailable(ftxui)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)

# Clang-tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()

add_executable(
  CPGen
  src/main.cpp src/CLI/CLI.cpp src/FileSystemWriter/FileSystemWriter.cpp
  src/ProjectGenerator/ProjectGenerator.cpp
  src/TemplateEngine/TemplateEngine.cpp)

target_include_directories(CPGen PRIVATE include)
target_link_libraries(
  CPGen
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component)

add_custom_command(
  TARGET CPGen
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/templates
          ${CMAKE_BINARY_DIR}/templates)

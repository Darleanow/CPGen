cmake_minimum_required(VERSION 4.0)
project(
  {{PROJECT_NAME}} 
  VERSION 1.0.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD {{CXX_STANDARD}})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)

{{CLANG_TIDY}}

add_executable({{PROJECT_NAME}} src/main.cpp)

target_include_directories({{PROJECT_NAME}} PRIVATE include)

{{LINK_LIBS}}

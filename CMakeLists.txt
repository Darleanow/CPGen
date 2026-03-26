cmake_minimum_required(VERSION 3.31) # Fallback due to CI
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
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.12.0)

FetchContent_MakeAvailable(googletest json)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)

# Clang-tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()

add_library(
  CPGenSource
  src/CLI/CLI.cpp
  src/FileSystemWriter/FileSystemWriter.cpp
  src/ProjectGenerator/ProjectGenerator.cpp
  src/TemplateEngine/TemplateEngine.cpp
  src/System/System.cpp
  src/TUI/Components/Basic/Checkbox.cpp
  src/TUI/Components/Basic/Input.cpp
  src/TUI/Components/Groups/ComponentGroup.cpp
  src/TUI/View/View.cpp)

target_link_libraries(CPGenSource PRIVATE nlohmann_json::nlohmann_json)

add_executable(CPGen src/main.cpp)

target_include_directories(CPGenSource
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CPGen PRIVATE CPGenSource)

add_custom_command(
  TARGET CPGen
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/templates
          ${CMAKE_BINARY_DIR}/templates)

enable_testing()

add_executable(test_suite tests/UTCLI.cpp)
target_include_directories(test_suite PRIVATE include)

target_link_libraries(
  test_suite
  PRIVATE GTest::gtest_main
  PRIVATE CPGenSource)

include(GoogleTest)
gtest_discover_tests(test_suite)

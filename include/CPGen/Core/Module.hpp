#pragma once

#include <string>

/**
 * @file Module.hpp
 * @brief Defines the FetchContent descriptor used by module definitions.
 * @author Darleanow
 */

/**
 * @struct FetchContent
 * @brief Describes a CMake FetchContent dependency to be downloaded at build time.
 *
 * When a module declares a FetchContent block in its JSON definition, it is
 * parsed into this struct and later injected into the generated CMakeLists.txt
 * via a @c FetchContent_Declare / @c FetchContent_MakeAvailable call pair.
 */
struct FetchContent {
  std::string module_name; ///< Name passed to @c FetchContent_Declare (e.g. @c "googletest").
  std::string url;         ///< Archive URL or Git repository used for the download.
};

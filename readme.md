# CPGen

[![Unit Tests](https://github.com/Darleanow/cpgen/actions/workflows/UT.yml/badge.svg?branch=main)](https://github.com/Darleanow/cpgen/actions/workflows/UT.yml)

A modern C++ project generator. Scaffold production-ready CMake projects with modules, tooling, and tests — from the CLI or an interactive TUI.

## Features

- **Full CMake generation** — `CMakeLists.txt` with modern best practices (FetchContent, compile options, clang-tidy integration)
- **Module system** — extensible JSON-based module definitions that inject dependencies, link targets, and test discovery
- **Tooling support** — `.clang-tidy`, `.clang-format` (with preset support), `.gitignore`
- **Interactive TUI** — terminal UI with checkboxes and text inputs for project configuration
- **Scaffold** — generates `src/main.cpp`, `tests/test_main.cpp`, `README.md` out of the box

## Installation

### Build from source

```bash
git clone https://github.com/Darleanow/cpgen.git
cd cpgen
cmake -B build -G Ninja
ninja -C build
```

The binary is at `build/CPGen`. Templates are auto-copied next to the executable.

## Usage

### CLI mode

```bash
# Minimal — generate a project with defaults (C++20, no modules)
CPGen -n MyProject -p /tmp

# Full example — C++23, gtest + spdlog, git, clang-tidy, clang-format with Google style
CPGen -n MyProject -p /tmp -s 23 -m gtest,spdlog -g -ct -cf -cfp Google

# Generate with test and executable targets
CPGen -n MyProject -p /tmp -m gtest -tar exe,test
```

### TUI mode

```bash
CPGen -u
```

Launches an interactive terminal interface where you can configure the project name, output path, and tooling options before generating.

### All flags

| Short | Long | Description |
|-------|------|-------------|
| `-h` | `--help` | Show help and exit |
| `-u` | `--tui` | Launch interactive TUI mode |
| `-n` | `--name` | Project name (must match `^\w+$`) |
| `-p` | `--path` | Output directory (must exist on disk) |
| `-s` | `--std` | C++ standard: `17`, `20`, or `23` (default: 20) |
| `-m` | `--modules` | Comma-separated modules: `-m gtest,spdlog` |
| `-tar` | `--targets` | Comma-separated target types: `-tar exe,test,lib` |
| `-g` | `--git` | Initialize a `.gitignore` |
| `-cf` | `--clang-format` | Generate `.clang-format` |
| `-cfp` | `--cf-preset` | Clang-format style preset (e.g. `Google`, `LLVM`) |
| `-ct` | `--clang-tidy` | Generate `.clang-tidy` and CMake integration |

## Available Modules

| Module | Description | Target |
|--------|-------------|--------|
| `gtest` | Google Test framework | `test` |
| `catch2` | Catch2 v3 test framework | `test` |
| `spdlog` | Fast C++ logging library | `main` |
| `fmt` | Modern formatting library | `main` |

### Adding custom modules

Create a JSON file at `templates/modules/<name>/<name>.json`:

```json
{
  "name": "mylib",
  "fetch_content": {
    "module_name": "mylib",
    "url": "https://github.com/org/mylib/archive/refs/tags/v1.0.0.zip"
  },
  "targets": {
    "main": {
      "link": ["mylib::mylib"]
    }
  }
}
```

Optional fields per target: `cmake_include` (e.g. `"GoogleTest"`), `post_link` (e.g. `"gtest_discover_tests(${TARGET_NAME})"`).

## Architecture

```
CPGen/
├── include/CPGen/
│   ├── CLI/            # Command-line argument parser
│   ├── Core/           # Config, Module, Resolved data types
│   ├── Pipeline/       # Emitter interface + CMake/Scaffold/Tooling emitters
│   ├── Resolvers/      # Module & project resolution from JSON definitions
│   ├── System/         # Path resolution, system utilities
│   └── TUI/            # Terminal UI components (Checkbox, Input, View)
├── src/                # Implementation files
├── templates/          # Module definitions + feature templates
└── tests/              # Google Test unit tests (93 tests)
```

## License

MIT

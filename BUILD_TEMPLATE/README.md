# Build System Template

A complete, genericized build system template extracted from the crucible project. This template provides a production-ready build system for C projects with support for multi-platform builds, CI/CD, and automated releases.

## Overview

This template includes:

- **Makefile** - Complete build system with cross-compilation support
- **Docker** - Multi-stage Dockerfile for containerized builds
- **GitHub Actions** - CI/CD workflows for automated testing and releases
- **Documentation** - Comprehensive build and contribution guides
- **Configuration** - Code formatting and gitignore templates

## Quick Start

### 1. Copy Template Files

Copy the template files to your project root:

```bash
# Copy all template files
cp -r BUILD_TEMPLATE/*.template .
cp -r BUILD_TEMPLATE/.github .
cp -r BUILD_TEMPLATE/docs .
cp BUILD_TEMPLATE/.gitignore.template .gitignore
cp BUILD_TEMPLATE/.clang-format.template .clang-format

# If you have assembly files
cp BUILD_TEMPLATE/asm/Makefile.template asm/Makefile
```

### 2. Replace Placeholders

Use a search-and-replace tool or script to replace all `{{PLACEHOLDER}}` values:

```bash
# Example using sed
find . -type f -name "*.template" -o -name "Makefile" -o -name "*.yml" -o -name "*.md" | \
  xargs sed -i 's/{{PROJECT_NAME}}/your-project-name/g'
```

Or use a templating tool like `envsubst`, `mustache`, or write a simple script.

### 3. Customize Configuration

Edit the Makefile and set your project-specific variables:

- Source files location
- Include directories
- Compiler flags
- Linker libraries
- Test commands

### 4. Remove .template Extensions

After customization, remove `.template` extensions:

```bash
find . -name "*.template" -exec sh -c 'mv "$1" "${1%.template}"' _ {} \;
```

## Template Variables

All template files use `{{PLACEHOLDER}}` syntax. Here are the key variables:

### Project Identification

- `{{PROJECT_NAME}}` - Project name (e.g., "myproject")
- `{{PROJECT_DESCRIPTION}}` - Short project description
- `{{PROJECT_OWNER}}` - GitHub org/user (e.g., "username")
- `{{PROJECT_REPO}}` - Repository name
- `{{TARGET_NAME}}` - Executable name (defaults to PROJECT_NAME)
- `{{COPYRIGHT_YEAR}}` - Copyright year

### Directories

- `{{SRC_DIR}}` - Source directory (default: "src")
- `{{BUILD_DIR}}` - Build output directory (default: "build")
- `{{RELEASE_DIR}}` - Release output directory (default: "release")
- `{{OUTPUT_DIR}}` - Assembly output directory (default: "../src")

### Build Configuration

- `{{INCLUDE_DIRS}}` - Include directories (e.g., "-Iccan -Iinclude")
- `{{CFLAGS}}` - Compiler flags (e.g., "-O3 -Wall -g -flto=auto")
- `{{LDLIBS}}` - Linker libraries (e.g., "-lm -lpthread")
- `{{CHANGELOG_PATH}}` - Path to CHANGELOG.md (default: "docs/CHANGELOG.md")

### CI/CD

- `{{MAIN_BRANCH}}` - Main branch name (default: "main")
- `{{DEVELOP_BRANCH}}` - Develop branch name (default: "develop")
- `{{CI_WORKFLOW}}` - CI workflow name (default: "CI")
- `{{TEST_COMMAND}}` - Test command (default: "make test")
- `{{BUILD_DIR}}` - Build directory for CI (default: "build")

### Docker

- `{{DOCKER_IMAGE}}` - Docker image for macOS builds (if applicable)
- `{{BASE_IMAGE}}` - Base Docker image (default: "ubuntu:22.04")

### Assembly (if applicable)

- `{{AS}}` - Assembler command (e.g., "mads", "nasm")
- `{{HEADER_TARGETS}}` - Header file targets
- `{{HEADER_NAME}}` - Header file name
- `{{BINARY_NAME}}` - Binary file name
- `{{TEMPLATE_NAME}}` - Template file name
- `{{SOURCE_FILES}}` - Assembly source files

## File Structure

```
BUILD_TEMPLATE/
├── README.md                    # This file
├── CONFIGURATION.md             # Detailed configuration reference
├── EXAMPLES.md                  # Usage examples
├── Makefile.template            # Main build Makefile
├── Dockerfile.release.template  # Docker release build
├── .gitignore.template          # Build artifacts ignore patterns
├── .clang-format.template       # Code formatting config
├── asm/
│   └── Makefile.template        # Assembly build Makefile
├── .github/
│   └── workflows/
│       ├── ci.yml.template
│       ├── release.yml.template
│       ├── auto-merge.yml.template
│       ├── merge-develop.yml.template
│       └── README.md.template
└── docs/
    ├── BUILDING.md.template
    ├── CI_CD.md.template
    └── CONTRIBUTING.md.template
```

## Usage Examples

### Simple C Project

For a simple C project with minimal dependencies:

1. Set `{{INCLUDE_DIRS}}` to empty or your include paths
2. Set `{{LDLIBS}}` to required libraries (e.g., "-lm")
3. Remove assembly Makefile if not needed
4. Customize test target or leave empty

### Multi-Platform Project

For projects requiring cross-compilation:

1. Ensure cross-compilers are installed or use Docker
2. Configure `{{DOCKER_IMAGE}}` for macOS builds
3. Customize platform-specific build targets in Makefile
4. Test builds on each target platform

### Project with Assembly

For projects with assembly code:

1. Configure `{{AS}}` assembler command
2. Set up assembly source files and targets
3. Configure header generation rules
4. Integrate assembly build into main Makefile

## Customization Guide

### 1. Source Files

The Makefile supports two methods for specifying source files:

**Option 1: Auto-discovery** (default)
```makefile
SRC = $(wildcard $(SRC_DIR)/*.c)
```

**Option 2: Explicit list**
```makefile
SRC = \
  $(SRC_DIR)/file1.c \
  $(SRC_DIR)/file2.c \
  $(SRC_DIR)/main.c
```

### 2. Dependencies

Add dependency rules in the Makefile:

```makefile
$(ODIR)/file.o: $(SRC_DIR)/file.c $(SRC_DIR)/file.h $(SRC_DIR)/other.h
```

### 3. Test Target

Configure the test target in the Makefile:

```makefile
TEST_TARGET ?= $(TARGET) --test
```

Or leave empty to disable tests.

### 4. Release Platforms

Modify the `release` target to include/exclude platforms:

```makefile
release: release-linux-amd64 release-linux-arm64
```

## Migration from Crucible

If you're migrating from the crucible project:

1. Copy all template files
2. Replace `crucible` with your project name
3. Replace `AtariFoundry` with your GitHub org/user
4. Update source file lists
5. Customize include directories and libraries
6. Update test commands
7. Remove assembly Makefile if not needed

## Prerequisites

- **GNU Make** 3.81+ or compatible
- **C Compiler** (GCC, Clang, or MSVC)
- **Git** for version control
- **Docker** (optional, for macOS builds)
- **GitHub CLI** (optional, for automated releases)

## Documentation

- **[CONFIGURATION.md](CONFIGURATION.md)** - Detailed variable documentation
- **[EXAMPLES.md](EXAMPLES.md)** - Usage examples for different project types
- **[docs/BUILDING.md.template](docs/BUILDING.md.template)** - Build instructions template
- **[docs/CONTRIBUTING.md.template](docs/CONTRIBUTING.md.template)** - Contribution guide template

## Support

For issues or questions:

1. Check the documentation files
2. Review the examples in EXAMPLES.md
3. Examine the original crucible project for reference
4. Open an issue in the repository

## License

This build system template is provided as-is. Customize it according to your project's needs and license requirements.

## Credits

This template was extracted and genericized from the [crucible](https://github.com/AtariFoundry/crucible) project build system.

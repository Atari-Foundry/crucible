# Configuration Reference

This document provides detailed documentation for all template variables and configuration options.

## Template Variables

### Project Identification

#### `{{PROJECT_NAME}}`
- **Description**: The name of your project
- **Example**: `"myproject"`, `"crucible"`, `"my-tool"`
- **Used in**: All files (Makefile, workflows, documentation)
- **Required**: Yes

#### `{{PROJECT_DESCRIPTION}}`
- **Description**: Short description of the project
- **Example**: `"A small 6502 simulator"`, `"Command-line utility for processing files"`
- **Used in**: Documentation, help text
- **Required**: Yes

#### `{{PROJECT_OWNER}}`
- **Description**: GitHub organization or username
- **Example**: `"AtariFoundry"`, `"username"`, `"myorg"`
- **Used in**: GitHub workflows, documentation, repository URLs
- **Required**: Yes

#### `{{PROJECT_REPO}}`
- **Description**: Repository name (usually same as PROJECT_NAME)
- **Example**: `"crucible"`, `"myproject"`
- **Used in**: Documentation, repository URLs
- **Required**: Yes

#### `{{TARGET_NAME}}`
- **Description**: Executable name (defaults to PROJECT_NAME)
- **Example**: `"crucible"`, `"myapp"`
- **Used in**: Makefile, build outputs, release files
- **Required**: No (defaults to PROJECT_NAME)

#### `{{COPYRIGHT_YEAR}}`
- **Description**: Copyright year
- **Example**: `"2026"`, `"2024"`
- **Used in**: Makefile header
- **Required**: No (can use current year)

### Directory Configuration

#### `{{SRC_DIR}}`
- **Description**: Source code directory
- **Default**: `"src"`
- **Example**: `"src"`, `"source"`, `"lib"`
- **Used in**: Makefile
- **Required**: No (defaults to "src")

#### `{{BUILD_DIR}}`
- **Description**: Build output directory
- **Default**: `"build"`
- **Example**: `"build"`, `"out"`, `"bin"`
- **Used in**: Makefile, workflows, documentation
- **Required**: No (defaults to "build")

#### `{{RELEASE_DIR}}`
- **Description**: Release binaries directory
- **Default**: `"release"`
- **Example**: `"release"`, `"dist"`, `"artifacts"`
- **Used in**: Makefile, Dockerfile, documentation
- **Required**: No (defaults to "release")

#### `{{OUTPUT_DIR}}`
- **Description**: Assembly output directory (for assembly builds)
- **Default**: `"../src"`
- **Example**: `"../src"`, `"../include"`
- **Used in**: asm/Makefile
- **Required**: Only if using assembly builds

### Build Configuration

#### `{{INCLUDE_DIRS}}`
- **Description**: Include directories for compiler
- **Default**: `""` (empty)
- **Example**: `"-Iccan"`, `"-Iinclude -Ilib"`, `"-I./include -I./third_party"`
- **Used in**: Makefile, Dockerfile
- **Required**: No (can be empty)

#### `{{CFLAGS}}`
- **Description**: Compiler flags
- **Default**: `"-O3 -Wall -g -flto=auto"`
- **Example**: `"-O3 -Wall -g -flto=auto"`, `"-O2 -Wall -Wextra"`
- **Used in**: Makefile, Dockerfile
- **Required**: No (has defaults)

**Common flag combinations**:
- Debug: `"-g -O0 -Wall -Wextra -Wpedantic"`
- Release: `"-O3 -flto -DNDEBUG"`
- Sanitizer: `"-g -O0 -fsanitize=address"`

#### `{{LDLIBS}}`
- **Description**: Linker libraries
- **Default**: `"-lm"` (math library)
- **Example**: `"-lm"`, `"-lm -lpthread"`, `"-lssl -lcrypto"`
- **Used in**: Makefile, Dockerfile
- **Required**: No (can be empty)

**Common libraries**:
- Math: `"-lm"`
- Threading: `"-lpthread"`
- SSL: `"-lssl -lcrypto"`
- Network: `"-lnetwork"` (platform-specific)

#### `{{CHANGELOG_PATH}}`
- **Description**: Path to CHANGELOG.md file
- **Default**: `"docs/CHANGELOG.md"`
- **Example**: `"docs/CHANGELOG.md"`, `"CHANGELOG.md"`
- **Used in**: Makefile (github-release target)
- **Required**: No (defaults to "docs/CHANGELOG.md")

### CI/CD Configuration

#### `{{MAIN_BRANCH}}`
- **Description**: Main/production branch name
- **Default**: `"main"`
- **Example**: `"main"`, `"master"`, `"production"`
- **Used in**: GitHub workflows, documentation
- **Required**: No (defaults to "main")

#### `{{DEVELOP_BRANCH}}`
- **Description**: Development branch name
- **Default**: `"develop"`
- **Example**: `"develop"`, `"dev"`, `"development"`
- **Used in**: GitHub workflows, documentation
- **Required**: No (defaults to "develop")

#### `{{CI_WORKFLOW}}`
- **Description**: Name of CI workflow file
- **Default**: `"CI"`
- **Example**: `"CI"`, `"Continuous Integration"`
- **Used in**: Auto-merge workflows
- **Required**: No (defaults to "CI")

#### `{{TEST_COMMAND}}`
- **Description**: Command to run tests
- **Default**: `"make test"`
- **Example**: `"make test"`, `"./scripts/test.sh"`, `"pytest"`
- **Used in**: CI workflow
- **Required**: No (defaults to "make test")

### Docker Configuration

#### `{{DOCKER_IMAGE}}`
- **Description**: Docker image for macOS cross-compilation
- **Default**: `""` (empty, macOS builds disabled)
- **Example**: `"ghcr.io/shepherdjerred/macos-cross-compiler:latest"`
- **Used in**: Makefile (macOS release targets)
- **Required**: Only if building macOS binaries

#### `{{BASE_IMAGE}}`
- **Description**: Base Docker image for builds
- **Default**: `"ubuntu:22.04"`
- **Example**: `"ubuntu:22.04"`, `"ubuntu:20.04"`, `"debian:bullseye"`
- **Used in**: Dockerfile.release
- **Required**: No (defaults to "ubuntu:22.04")

### Assembly Configuration (Optional)

#### `{{AS}}`
- **Description**: Assembler command
- **Default**: `"mads"` (example)
- **Example**: `"mads"`, `"nasm"`, `"as"`
- **Used in**: asm/Makefile
- **Required**: Only if using assembly builds

#### `{{HEADER_TARGETS}}`
- **Description**: List of header files to generate
- **Example**: `"../src/file_bin.h ../src/file_atari.h"`
- **Used in**: asm/Makefile
- **Required**: Only if using assembly builds

## Configuration Examples

### Minimal C Project

```makefile
PROJECT_NAME = mytool
INCLUDE_DIRS = 
CFLAGS = -O2 -Wall
LDLIBS = 
TEST_TARGET = 
```

### Project with External Libraries

```makefile
PROJECT_NAME = network-tool
INCLUDE_DIRS = -I./include -I./third_party
CFLAGS = -O3 -Wall -g
LDLIBS = -lm -lpthread -lssl -lcrypto
TEST_TARGET = ./tests/run_tests.sh
```

### Project with Assembly

```makefile
PROJECT_NAME = embedded-tool
INCLUDE_DIRS = -I./include
CFLAGS = -O3 -Wall -g
LDLIBS = -lm
AS = nasm
OUTPUT_DIR = ../src
```

## Variable Replacement

### Manual Replacement

Use search-and-replace in your editor or command-line tools:

```bash
# Using sed
find . -type f \( -name "*.template" -o -name "Makefile" -o -name "*.yml" -o -name "*.md" \) \
  -exec sed -i 's/{{PROJECT_NAME}}/myproject/g' {} \;

# Using find and replace in editor
# Search: {{PROJECT_NAME}}
# Replace: myproject
```

### Automated Replacement Script

Create a script to replace all variables:

```bash
#!/bin/bash
PROJECT_NAME="myproject"
PROJECT_DESCRIPTION="My awesome project"
PROJECT_OWNER="myusername"
PROJECT_REPO="myproject"
TARGET_NAME="myproject"
SRC_DIR="src"
BUILD_DIR="build"
RELEASE_DIR="release"
INCLUDE_DIRS="-I./include"
CFLAGS="-O3 -Wall -g"
LDLIBS="-lm"

find . -type f \( -name "*.template" -o -name "Makefile" -o -name "*.yml" -o -name "*.md" \) | while read file; do
  sed -i \
    -e "s/{{PROJECT_NAME}}/$PROJECT_NAME/g" \
    -e "s/{{PROJECT_DESCRIPTION}}/$PROJECT_DESCRIPTION/g" \
    -e "s/{{PROJECT_OWNER}}/$PROJECT_OWNER/g" \
    -e "s/{{PROJECT_REPO}}/$PROJECT_REPO/g" \
    -e "s/{{TARGET_NAME}}/$TARGET_NAME/g" \
    -e "s/{{SRC_DIR}}/$SRC_DIR/g" \
    -e "s/{{BUILD_DIR}}/$BUILD_DIR/g" \
    -e "s/{{RELEASE_DIR}}/$RELEASE_DIR/g" \
    -e "s/{{INCLUDE_DIRS}}/$INCLUDE_DIRS/g" \
    -e "s/{{CFLAGS}}/$CFLAGS/g" \
    -e "s/{{LDLIBS}}/$LDLIBS/g" \
    "$file"
done
```

### Using envsubst

If using environment variables:

```bash
export PROJECT_NAME="myproject"
export PROJECT_DESCRIPTION="My project"
# ... set other variables ...

find . -name "*.template" | while read file; do
  envsubst < "$file" > "${file%.template}"
done
```

## Validation Checklist

After replacing variables, verify:

- [ ] All `{{PLACEHOLDER}}` values replaced
- [ ] Makefile compiles without errors
- [ ] Source files are correctly specified
- [ ] Include directories are correct
- [ ] Linker libraries are correct
- [ ] Test target works (if configured)
- [ ] CI workflows reference correct branches
- [ ] Documentation reflects project name
- [ ] Release targets build successfully
- [ ] Dockerfile builds (if using Docker)

## Troubleshooting

### Variable Not Replaced

- Check for typos in placeholder names
- Ensure file is included in replacement script
- Verify placeholder syntax: `{{VARIABLE_NAME}}`

### Build Fails After Replacement

- Check for special characters in variable values
- Verify paths are correct (use absolute paths if needed)
- Check for missing quotes in Makefile variables

### CI Workflow Fails

- Verify branch names match your repository
- Check workflow file syntax (YAML is sensitive)
- Ensure test command is correct

## Advanced Configuration

### Conditional Compilation

Add conditional features in Makefile:

```makefile
ifdef DEBUG
  CFLAGS += -DDEBUG -g -O0
else
  CFLAGS += -DNDEBUG -O3
endif
```

### Platform-Specific Flags

```makefile
ifeq ($(OS),Windows_NT)
  TARGET_EXT = .exe
  LDLIBS += -lws2_32
else
  TARGET_EXT = 
  LDLIBS += -lpthread
endif
```

### Custom Build Targets

Add project-specific targets:

```makefile
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET_NAME)
```

## See Also

- [README.md](README.md) - Quick start guide
- [EXAMPLES.md](EXAMPLES.md) - Usage examples
- [docs/BUILDING.md.template](docs/BUILDING.md.template) - Build documentation

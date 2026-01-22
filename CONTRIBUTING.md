# Contributing to Crucible

Thank you for your interest in contributing to Crucible! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [How to Contribute](#how-to-contribute)
3. [Development Setup](#development-setup)
4. [Coding Standards](#coding-standards)
5. [Testing](#testing)
6. [Git Workflow](#git-workflow)
7. [Pull Request Process](#pull-request-process)
8. [License](#license)
9. [Getting Help](#getting-help)

## Code of Conduct

- Be respectful and considerate of others
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Respect different viewpoints and experiences

## How to Contribute

There are many ways to contribute to Crucible:

### Reporting Bugs

If you find a bug, please open an issue on GitHub with:

- **Clear title** describing the issue
- **Description** of the problem
- **Steps to reproduce** the issue
- **Expected behavior** vs **actual behavior**
- **Environment** (OS, compiler version, etc.)
- **Minimal example** if possible (XEX file, ATR image, etc.)

### Suggesting Features

Feature suggestions are welcome! Please open an issue with:

- **Clear title** describing the feature
- **Description** of the proposed feature
- **Use case** explaining why this would be useful
- **Possible implementation** approach (if you have ideas)

### Code Contributions

See [Development Setup](#development-setup) and [Git Workflow](#git-workflow) below for details on contributing code.

## Development Setup

### Prerequisites

- **C Compiler**: GCC 4.8+ or Clang
- **Make**: GNU Make 3.81+
- **Git**: For version control
- **Standard C Library**: POSIX-compliant

### Getting Started

1. **Fork the repository** on GitHub

2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/crucible.git
   cd crucible
   ```

3. **Add upstream remote**:
   ```bash
   git remote add upstream https://github.com/AtariFoundry/crucible.git
   ```

4. **Create a development branch**:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/your-bug-fix
   ```

5. **Build the project**:
   ```bash
   make
   ```

6. **Run tests**:
   ```bash
   make test
   ```

### Build Options

For development, you may want to use debug flags:

```bash
# Debug build with all warnings
make CFLAGS="-Iccan -g -O0 -Wall -Wextra -Wpedantic"

# Or use address sanitizer
make CFLAGS="-Iccan -g -O0 -fsanitize=address" \
     LDLIBS="-lm -fsanitize=address"
```

See [docs/BUILDING.md](docs/BUILDING.md) for more build options.

## Coding Standards

### Code Style

Crucible uses **clang-format** for code formatting. The configuration is in `.clang-format`.

**Before committing**, format your code:

```bash
# Format all source files
find src -name "*.c" -o -name "*.h" | xargs clang-format -i

# Or format a specific file
clang-format -i src/your-file.c
```

### Style Guidelines

- **Indentation**: 4 spaces (no tabs)
- **Brace Style**: Allman (braces on new lines)
- **Line Length**: No hard limit, but keep lines readable
- **Naming**: 
  - Functions: `snake_case`
  - Types/Structs: `snake_case`
  - Constants: `UPPER_CASE` or `snake_case`
- **Comments**: Use `/* */` for block comments, `//` for single-line comments
- **Header Guards**: Use `#ifndef` / `#define` / `#endif`

### Code Organization

- **One function per file** when possible, or group related functions
- **Header files** should contain declarations, not implementations
- **Include order**: 
  1. Corresponding header (for .c files)
  2. System headers (`<stdio.h>`, etc.)
  3. Project headers (`"atari.h"`, etc.)
  4. Local headers

### Example

```c
/*
 * File: example.c
 * Description: Brief description of what this file does
 * Copyright (C) 2026 AtariFoundry.com
 */

#include "example.h"
#include <stdio.h>
#include <stdlib.h>
#include "atari.h"

static int helper_function(int value)
{
    return value * 2;
}

int public_function(int input)
{
    if (input < 0)
    {
        return -1;
    }
    
    return helper_function(input);
}
```

## Testing

### Running Tests

Always run tests before submitting a pull request:

```bash
make test
```

### Writing Tests

If you add new functionality, please add tests:

1. **Unit tests**: Test individual functions
2. **Integration tests**: Test with real XEX files or ATR images
3. **Regression tests**: Test cases that previously failed

Test files should be placed in `testfiles/` directory. See existing test files for examples.

### Test Requirements

- All tests must pass before merging
- New features should include tests
- Bug fixes should include regression tests

## Git Workflow

### Branch Structure

- **`main`** - Production/release branch (stable)
- **`develop`** - Development branch (integration)

### Workflow

1. **Create a feature branch** from `develop`:
   ```bash
   git checkout develop
   git pull upstream develop
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** and commit:
   ```bash
   git add .
   git commit -m "Add feature: description of changes"
   ```

3. **Keep your branch updated**:
   ```bash
   git checkout develop
   git pull upstream develop
   git checkout feature/your-feature-name
   git rebase develop
   ```

4. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

### Commit Messages

Write clear, descriptive commit messages:

```
Short summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain what and why, not how.

- Bullet points for multiple changes
- Reference issues: Fixes #123
```

**Good examples**:
- `Fix: Handle empty ATR files correctly`
- `Add: Support for SpartaDOS directory listing`
- `Refactor: Simplify CIO device registration`

**Bad examples**:
- `fix`
- `WIP`
- `changes`

## Pull Request Process

### Before Submitting

1. ✅ **Code compiles** without warnings
2. ✅ **Code is formatted** with clang-format
3. ✅ **Tests pass** (`make test`)
4. ✅ **Branch is up to date** with `develop`
5. ✅ **No merge conflicts** with `develop`

### Submitting a Pull Request

1. **Push your branch** to your fork:
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Open a Pull Request** on GitHub:
   - Base branch: `develop`
   - Compare branch: `feature/your-feature-name`
   - Fill out the PR template (if available)

3. **PR Description should include**:
   - What changes were made
   - Why the changes were made
   - How to test the changes
   - Related issues (e.g., "Fixes #123")

### Review Process

- Maintainers will review your PR
- Address any feedback or requested changes
- Once approved, your PR will be merged to `develop`
- After CI passes, `develop` is automatically merged to `main`

### PR Checklist

- [ ] Code follows style guidelines
- [ ] Code is commented where necessary
- [ ] Tests pass locally
- [ ] Tests added for new functionality
- [ ] Documentation updated (if needed)
- [ ] No merge conflicts
- [ ] Branch is up to date with `develop`

## License

By contributing to Crucible, you agree that your contributions will be licensed under the **GNU General Public License v2 or later** (GPL-2.0+).

This means:
- Your contributions become part of the project
- The project remains open source
- All contributions must be compatible with GPL-2.0+

See [LICENSE](LICENSE) for the full license text.

## Getting Help

### Documentation

- **[User Guide](docs/USER_GUIDE.md)** - Usage instructions
- **[API Reference](docs/API_REFERENCE.md)** - Developer API
- **[Architecture](docs/ARCHITECTURE.md)** - System design
- **[Building Guide](docs/BUILDING.md)** - Build instructions

### Communication

- **GitHub Issues**: For bug reports and feature requests
- **GitHub Discussions**: For questions and general discussion (if enabled)

### Questions?

If you're unsure about something:
1. Check the documentation
2. Search existing issues
3. Open a new issue with the "question" label

## Thank You!

Your contributions make Crucible better for everyone. We appreciate your time and effort!

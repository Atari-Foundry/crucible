# Usage Examples

This document provides practical examples of using the build system template for different project types.

## Example 1: Simple C Utility

A minimal command-line utility with no external dependencies.

### Configuration

```makefile
PROJECT_NAME = myutil
PROJECT_DESCRIPTION = "A simple command-line utility"
PROJECT_OWNER = myusername
PROJECT_REPO = myutil
TARGET_NAME = myutil
SRC_DIR = src
INCLUDE_DIRS = 
CFLAGS = -O2 -Wall -g
LDLIBS = 
TEST_TARGET = ./tests/run_tests.sh
```

### Steps

1. Copy template files
2. Replace placeholders with above values
3. Remove assembly Makefile (not needed)
4. Set up simple test script or leave TEST_TARGET empty
5. Build: `make`

### Result

- Single executable: `build/myutil`
- No external dependencies
- Simple build process

## Example 2: C Project with Math Library

A project that uses the standard math library.

### Configuration

```makefile
PROJECT_NAME = calculator
PROJECT_DESCRIPTION = "Scientific calculator"
PROJECT_OWNER = myorg
PROJECT_REPO = calculator
TARGET_NAME = calc
SRC_DIR = src
INCLUDE_DIRS = -I./include
CFLAGS = -O3 -Wall -g -flto=auto
LDLIBS = -lm
TEST_TARGET = make test
```

### Steps

1. Copy template files
2. Replace placeholders
3. Ensure math library is available
4. Add test target in Makefile:
   ```makefile
   test: $(TARGET)
       @./build/calc --test
   ```
5. Build: `make`

### Result

- Executable with math functions
- Optimized build with LTO
- Test target configured

## Example 3: Multi-Platform Network Tool

A network utility requiring cross-compilation for multiple platforms.

### Configuration

```makefile
PROJECT_NAME = nettool
PROJECT_DESCRIPTION = "Network diagnostic tool"
PROJECT_OWNER = myorg
PROJECT_REPO = nettool
TARGET_NAME = nettool
SRC_DIR = src
INCLUDE_DIRS = -I./include -I./third_party
CFLAGS = -O3 -Wall -g -flto=auto
LDLIBS = -lpthread -lssl -lcrypto
TEST_TARGET = ./scripts/test.sh
DOCKER_IMAGE = ghcr.io/shepherdjerred/macos-cross-compiler:latest
```

### Steps

1. Copy all template files including Dockerfile
2. Replace placeholders
3. Install cross-compilers:
   ```bash
   sudo apt-get install gcc-aarch64-linux-gnu mingw-w64
   ```
4. Configure test script
5. Build all platforms: `make release`

### Result

- Binaries for Linux (amd64, arm64), Windows, macOS
- Network libraries linked
- Cross-compilation support

## Example 4: Embedded Project with Assembly

A project with both C and assembly code.

### Configuration

**Main Makefile:**
```makefile
PROJECT_NAME = embedded-app
PROJECT_DESCRIPTION = "Embedded application"
PROJECT_OWNER = myorg
PROJECT_REPO = embedded-app
TARGET_NAME = app
SRC_DIR = src
INCLUDE_DIRS = -I./include
CFLAGS = -O3 -Wall -g
LDLIBS = -lm
```

**asm/Makefile:**
```makefile
AS = nasm
ASM_DIR = .
OUTPUT_DIR = ../src
```

### Steps

1. Copy all template files including asm/Makefile
2. Replace placeholders
3. Configure assembly build rules:
   ```makefile
   all: ../src/bootcode.h
   
   ../src/bootcode.h: bootcode.bin template.h
       cp -f template.h $@
       xxd -i $< | sed -e 's/^  */    /' >> $@
   
   bootcode.bin: bootcode.asm
       $(AS) -f bin $< -o $@
   ```
4. Integrate assembly build into main Makefile:
   ```makefile
   all: asm-build $(TARGET)
   
   asm-build:
       $(MAKE) -C asm
   ```

### Result

- C and assembly code integrated
- Header files generated from binaries
- Combined build process

## Example 5: Library Project

A static library project (no main executable).

### Configuration

```makefile
PROJECT_NAME = mylib
PROJECT_DESCRIPTION = "Utility library"
PROJECT_OWNER = myorg
PROJECT_REPO = mylib
TARGET_NAME = libmylib.a
SRC_DIR = src
INCLUDE_DIRS = -I./include
CFLAGS = -O3 -Wall -g -fPIC
LDLIBS = 
```

### Modified Makefile Targets

```makefile
# Build static library instead of executable
$(TARGET): $(OBJS) | $(BDIR)
	ar rcs $@ $^

# Install target
install: $(TARGET)
	install -d /usr/local/lib
	install -d /usr/local/include
	install -m 644 $(TARGET) /usr/local/lib/
	install -m 644 include/*.h /usr/local/include/
```

### Steps

1. Copy template files
2. Replace placeholders
3. Modify Makefile to build library instead of executable
4. Add install target
5. Build: `make install`

### Result

- Static library: `build/libmylib.a`
- Headers installed
- Usable by other projects

## Example 6: Project with Custom Test Framework

A project using a custom test framework.

### Configuration

```makefile
PROJECT_NAME = testapp
PROJECT_DESCRIPTION = "Application with custom tests"
PROJECT_OWNER = myorg
PROJECT_REPO = testapp
TARGET_NAME = app
SRC_DIR = src
INCLUDE_DIRS = -I./include
CFLAGS = -O2 -Wall -g
LDLIBS = -lm
TEST_TARGET = ./test_runner --suite all
```

### Custom Test Target

```makefile
test: $(TARGET)
	@echo "Running custom test suite..."
	@$(TEST_TARGET)
	@echo "Tests completed"
```

### Steps

1. Copy template files
2. Replace placeholders
3. Create test runner script or binary
4. Configure TEST_TARGET
5. Run tests: `make test`

### Result

- Custom test framework integrated
- Tests run via make
- CI runs tests automatically

## Example 7: Project with Multiple Source Directories

A project with source files in multiple directories.

### Configuration

```makefile
PROJECT_NAME = complex-app
PROJECT_DESCRIPTION = "Complex application"
PROJECT_OWNER = myorg
PROJECT_REPO = complex-app
TARGET_NAME = app
SRC_DIR = src
INCLUDE_DIRS = -I./src -I./lib -I./third_party
CFLAGS = -O3 -Wall -g
LDLIBS = -lm -lpthread
```

### Modified Source Discovery

```makefile
# Collect sources from multiple directories
SRC = \
  $(wildcard $(SRC_DIR)/*.c) \
  $(wildcard lib/*.c) \
  $(wildcard third_party/utils/*.c)

# Adjust object file paths
OBJS = $(SRC:%.c=$(ODIR)/%.o)

# Create object directory structure
$(ODIR)/%.o: %.c | $(ODIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
```

### Steps

1. Copy template files
2. Replace placeholders
3. Modify source file discovery
4. Adjust object file rules
5. Build: `make`

### Result

- Sources from multiple directories
- Proper include paths
- Organized build output

## Example 8: Minimal CI/CD Setup

A project with minimal CI/CD requirements.

### Configuration

**GitHub Workflows:**
- Only `ci.yml` enabled
- `auto-merge.yml` and `merge-develop.yml` disabled
- `release.yml` disabled (manual releases)

### Steps

1. Copy template files
2. Replace placeholders
3. Delete or disable unused workflows:
   ```bash
   rm .github/workflows/auto-merge.yml.template
   rm .github/workflows/merge-develop.yml.template
   ```
4. Modify `ci.yml` to only build (no tests if not configured)
5. Push to trigger CI

### Result

- Basic CI verification
- No automatic merging
- Manual release process

## Example 9: Full-Featured Project

A complete project with all features enabled.

### Configuration

```makefile
PROJECT_NAME = fullapp
PROJECT_DESCRIPTION = "Full-featured application"
PROJECT_OWNER = myorg
PROJECT_REPO = fullapp
TARGET_NAME = fullapp
SRC_DIR = src
INCLUDE_DIRS = -I./include -I./third_party
CFLAGS = -O3 -Wall -g -flto=auto
LDLIBS = -lm -lpthread -lssl -lcrypto
TEST_TARGET = make test
DOCKER_IMAGE = ghcr.io/shepherdjerred/macos-cross-compiler:latest
CHANGELOG_PATH = docs/CHANGELOG.md
```

### Steps

1. Copy all template files
2. Replace all placeholders
3. Set up CHANGELOG.md with [Unreleased] section
4. Configure all workflows
5. Install prerequisites:
   - Cross-compilers
   - Docker
   - GitHub CLI
6. Build: `make release`
7. Release: `make github-release`

### Result

- Full multi-platform support
- Automated CI/CD
- Automated releases
- Complete documentation

## Example 10: Migration from Existing Project

Migrating an existing C project to use this template.

### Original Project Structure

```
old-project/
├── source/
│   ├── main.c
│   ├── util.c
│   └── util.h
├── Makefile (simple)
└── README.md
```

### Migration Steps

1. **Backup original project**
   ```bash
   cp -r old-project old-project-backup
   ```

2. **Copy template files**
   ```bash
   cp BUILD_TEMPLATE/*.template .
   cp -r BUILD_TEMPLATE/.github .
   cp -r BUILD_TEMPLATE/docs .
   ```

3. **Replace placeholders**
   - PROJECT_NAME = old-project
   - SRC_DIR = source
   - Update source file list in Makefile

4. **Migrate source files** (if needed)
   ```bash
   # If source/ needs to become src/
   mv source src
   ```

5. **Update Makefile source list**
   ```makefile
   SRC = \
     src/main.c \
     src/util.c
   ```

6. **Test build**
   ```bash
   make clean
   make
   ```

7. **Update documentation**
   - Replace project-specific examples
   - Update README.md

### Result

- Modern build system
- Multi-platform support
- CI/CD integration
- Improved documentation

## Common Customizations

### Adding a Version Target

```makefile
version:
	@echo "$(PROJECT_NAME) version $(VERSION)"
	@git describe --tags --always 2>/dev/null || echo "unknown"
```

### Adding an Install Target

```makefile
install: $(TARGET)
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET_NAME)

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET_NAME)
```

### Adding Debug/Release Variants

```makefile
debug: CFLAGS = $(INCLUDE_DIRS) -g -O0 -Wall -Wextra
debug: $(TARGET)

release: CFLAGS = $(INCLUDE_DIRS) -O3 -flto -DNDEBUG
release: clean $(TARGET)
```

### Platform-Specific Builds

```makefile
linux: $(TARGET)
macos: 
	$(MAKE) CC=clang $(TARGET)
windows:
	$(MAKE) CC=x86_64-w64-mingw32-gcc TARGET=$(TARGET).exe
```

## Troubleshooting Examples

### Build Fails: Missing Include

**Problem**: `fatal error: header.h: No such file or directory`

**Solution**: Add include directory:
```makefile
INCLUDE_DIRS = -I./include -I./missing/path
```

### Link Fails: Missing Library

**Problem**: `undefined reference to 'function'`

**Solution**: Add library to LDLIBS:
```makefile
LDLIBS = -lm -lmissinglib
```

### Test Target Not Working

**Problem**: `make test` does nothing

**Solution**: Configure TEST_TARGET:
```makefile
TEST_TARGET = $(TARGET) --test
```

Or add custom test target:
```makefile
test: $(TARGET)
	@./scripts/run_tests.sh
```

## See Also

- [README.md](README.md) - Quick start guide
- [CONFIGURATION.md](CONFIGURATION.md) - Configuration reference
- [docs/BUILDING.md.template](docs/BUILDING.md.template) - Build documentation

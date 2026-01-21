# Building Crucible

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Quick Build](#quick-build)
3. [Build Options](#build-options)
4. [Platform-Specific Instructions](#platform-specific-instructions)
5. [Troubleshooting](#troubleshooting)

## Prerequisites

### Required

- **C Compiler**: GCC 4.8+ or compatible (Clang, MSVC)
- **Make**: GNU Make 3.81+ or compatible
- **Standard C Library**: POSIX-compliant C library
- **Math Library**: Standard math library (`libm`)

### Optional

- **Git**: For version control
- **Debugger**: GDB or compatible for debugging

## Quick Build

### Standard Build

```bash
# Clone or download source
cd crucible

# Build
make

# Executable will be in build/crucible
./build/crucible -h
```

### Clean Build

```bash
# Remove all build artifacts
make clean

# Rebuild
make
```

## Build Options

### Make Targets

- `make` or `make all` - Build project (default)
- `make clean` - Remove all build artifacts
- `make help` - Show build system help
- `make test` - Run automated tests

### Compiler Options

The Makefile uses these default flags:

```makefile
CFLAGS = -Iccan -O3 -Wall -g -flto
LDLIBS = -lm
```

**Flags Explained**:
- `-Iccan`: Include path for ccan library
- `-O3`: Maximum optimization
- `-Wall`: Enable all warnings
- `-g`: Include debug symbols
- `-flto`: Link-time optimization
- `-lm`: Link math library

### Custom Build Options

#### Debug Build

```bash
make CFLAGS="-g -O0 -Wall"
```

#### Release Build (No Debug Symbols)

```bash
make CFLAGS="-O3 -flto -DNDEBUG"
```

#### Custom Compiler

```bash
make CC=clang
```

#### Disable LTO

```bash
make CFLAGS="-Iccan -O3 -Wall -g"
```

## Build System Details

### Directory Structure

```
crucible/
├── build/          # Build output (created)
│   ├── obj/        # Object files
│   └── crucible    # Executable
├── src/            # Source files
├── ccan/           # C library headers
├── asm/            # Assembly sources
└── Makefile        # Build configuration
```

### Source Files

**Core**:
- `src/sim65.c` - CPU simulator
- `src/atari.c` - BIOS emulation
- `src/atcio.c` - CIO system
- `src/main.c` - Main program

**Devices**:
- `src/ataridos.c` - D: device
- `src/athost.c` - H: device
- `src/atrdev.c` - R: device
- `src/atsio.c` - SIO emulation

**Filesystem**:
- `src/atrfs.c` - ATR filesystem
- `src/dosfname.c` - DOS filename handling

**Hardware**:
- `src/hw.c` - Hardware registers

**Math**:
- `src/mathpack.c` - Math pack integration

### Dependencies

**Internal**:
- `ccan/` - C library headers (included in source)

**External**:
- Standard C library (`libc`)
- Math library (`libm`)
- No other external dependencies

### Build Process

1. **Compilation**: Each `.c` file compiled to `.o` object file
2. **Linking**: All object files linked into executable
3. **LTO**: Link-time optimization (if enabled) optimizes across files

## Platform-Specific Instructions

### Linux

**Requirements**:
- GCC or Clang
- Make
- Standard development tools

**Build**:
```bash
make
```

**Install** (optional):
```bash
sudo cp build/crucible /usr/local/bin/
```

### macOS

**Requirements**:
- Xcode Command Line Tools
- Make (included with Xcode)

**Build**:
```bash
make
```

**Note**: May need to install Xcode Command Line Tools:
```bash
xcode-select --install
```

### Windows

**Option 1: MinGW/MSYS2**

```bash
# Install MSYS2, then:
pacman -S mingw-w64-x86_64-gcc make

# Build
make CC=x86_64-w64-mingw32-gcc
```

**Option 2: Visual Studio**

1. Open Developer Command Prompt
2. Navigate to source directory
3. Use NMake or convert Makefile to Visual Studio project

**Option 3: WSL (Windows Subsystem for Linux)**

Use Linux build instructions in WSL environment.

### Cross-Compilation

**Example: Build for ARM**

```bash
make CC=arm-linux-gnueabihf-gcc
```

**Example: Build for 32-bit**

```bash
make CC=gcc CFLAGS="-Iccan -O3 -Wall -g -m32"
```

## Troubleshooting

### Build Errors

#### "Command not found: gcc"

**Solution**: Install GCC compiler
- Linux: `sudo apt-get install gcc` (Debian/Ubuntu)
- macOS: Install Xcode Command Line Tools
- Windows: Install MinGW or use WSL

#### "fatal error: sim65.h: No such file or directory"

**Solution**: Ensure you're in the project root directory
```bash
cd /path/to/crucible
make clean
make
```

#### "undefined reference to `sqrt`"

**Solution**: Link math library (should be automatic, but try):
```bash
make LDLIBS="-lm"
```

#### LTO Errors

**Solution**: Disable LTO
```bash
make CFLAGS="-Iccan -O3 -Wall -g"
```

### Runtime Errors

#### "can't open disk image"

**Solution**: Check file path and permissions
```bash
ls -l disk.atr
chmod 644 disk.atr
```

#### Segmentation Fault

**Solution**: Build with debug symbols and use debugger
```bash
make CFLAGS="-g -O0 -Wall"
gdb ./build/crucible
```

### Performance Issues

#### Slow Execution

**Solutions**:
- Enable optimizations: `make CFLAGS="-Iccan -O3 -flto -Wall"`
- Disable debug output: Remove `-d` flag
- Use cycle-based timing: `-o cycletime`

#### Large Binary Size

**Solution**: Strip debug symbols
```bash
strip build/crucible
```

## Advanced Build Configuration

### Custom Makefile Variables

Edit `Makefile` or override on command line:

```bash
# Custom compiler
make CC=clang

# Custom flags
make CFLAGS="-Iccan -O2 -Wall -g -DDEBUG"

# Custom build directory
make BDIR=output
```

### Separate Debug/Release Builds

```bash
# Debug build
mkdir -p build-debug
make BDIR=build-debug CFLAGS="-Iccan -g -O0 -Wall"

# Release build
mkdir -p build-release
make BDIR=build-release CFLAGS="-Iccan -O3 -flto -Wall -DNDEBUG"
```

### Static Linking

```bash
make LDFLAGS="-static"
```

### 32-bit Build

```bash
make CFLAGS="-Iccan -O3 -Wall -g -m32" LDFLAGS="-m32"
```

## Testing the Build

### Run Tests

```bash
make test
```

### Manual Test

```bash
# Test help
./build/crucible -h

# Test with sample program
./build/crucible testfiles/hello.xex
```

## Installation

### System-Wide Installation

**Linux/macOS**:
```bash
sudo cp build/crucible /usr/local/bin/
```

**User Installation**:
```bash
cp build/crucible ~/bin/
# Add ~/bin to PATH if not already
```

### Create Package

**Debian/Ubuntu**:
```bash
# Create .deb package (requires packaging tools)
dpkg-buildpackage -us -uc
```

## Development Build

### Enable All Warnings

```bash
make CFLAGS="-Iccan -Wall -Wextra -Wpedantic -g -O0"
```

### Address Sanitizer

```bash
make CFLAGS="-Iccan -g -O0 -fsanitize=address" LDLIBS="-lm -fsanitize=address"
```

### Valgrind Support

```bash
make CFLAGS="-Iccan -g -O0"
valgrind ./build/crucible program.xex
```

## See Also

- [User Guide](USER_GUIDE.md) - Usage instructions
- [Architecture](ARCHITECTURE.md) - System design
- [API Reference](API_REFERENCE.md) - Developer API

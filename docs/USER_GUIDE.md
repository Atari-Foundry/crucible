# Crucible User Guide

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Command-Line Options](#command-line-options)
5. [Device Support](#device-support)
6. [File Formats](#file-formats)
7. [Examples](#examples)
8. [Troubleshooting](#troubleshooting)

## Introduction

Crucible is a small 6502 simulator designed to debug and run command-line (console-only) Atari 8-bit programs. It provides a subset of the Atari BIOS and hardware emulation, making it ideal for testing and debugging Atari software without requiring a full emulator.

### Key Features

- **6502 CPU Emulation**: Full 6502 instruction set support
- **Atari BIOS Emulation**: CIO, DOS, and hardware register support
- **ATR Filesystem Access**: Read files from ATR disk images
- **Host Filesystem Access**: Access local files via H: device
- **Hardware Emulation**: POKEY, GTIA, ANTIC, PIA registers
- **Debugging Support**: Tracing, profiling, and label files
- **Cross-Platform**: Works on Linux, macOS, and Windows

## Installation

### Prerequisites

- GCC compiler (or compatible C compiler)
- Make
- Standard C library
- Math library (`-lm`)

### Building from Source

```bash
# Clone or download the source code
cd crucible

# Build the project
make

# The executable will be in build/crucible
./build/crucible -h
```

### Build Options

- `make` - Build the project
- `make clean` - Remove all build artifacts
- `make help` - Show build system help
- `make test` - Run automated tests

## Quick Start

### Running a Simple Program

```bash
# Run a XEX file
./build/crucible program.xex

# Run with debug output
./build/crucible -d program.xex

# Run with ATR disk image
./build/crucible -I disk.atr program.xex
```

### Basic Examples

```bash
# Print "HELLO" (if you have hello.xex)
./build/crucible testfiles/hello.xex

# Access host filesystem
./build/crucible -R /path/to/files program.xex

# Boot from ATR image
./build/crucible -I disk.atr
```

## Command-Line Options

### Basic Options

- `-h` - Show help message
- `-d` - Print debug messages to standard error
- `-b` - Use binary mode for standard input/output (no ATASCII translation)
- `-D` - Don't emulate DOS device

### File Options

- `-I <file>` - Load ATR disk image for SIO emulation
  - If no executable is given, boots from this image
  - Example: `-I disk.atr`

- `-R <path>` - Set root path for D: and H: devices
  - Sets the base directory for file access
  - Example: `-R /home/user/atari_files`

- `-r <addr>` - Load ROM at given address instead of XEX file
  - Address can be in decimal or hexadecimal (0x prefix)
  - Example: `-r 0xC000`

### Debugging Options

- `-e <level>` - Set error level
  - `none` or `n` - No error reporting
  - `mem` or `m` - Memory errors only
  - `full` or `f` - All errors (default)
  - Example: `-e mem`

- `-t <file>` - Store simulation trace to file
  - Records instruction execution trace
  - Example: `-t trace.txt`

- `-l <file>` - Load label file for trace output
  - Makes trace output more readable
  - Can specify multiple times (last one takes precedence)
  - Example: `-l labels.txt`

### Profiling Options

- `-p <file>` - Store profile information to file
  - Text format profile data
  - Example: `-p profile.txt`

- `-P <file>` - Read/write binary profile data
  - Can consolidate multiple profile runs
  - Example: `-P profile.bin`

### Advanced Options (`-o`)

- `ntsc` - Emulate NTSC machine timings (60Hz) - **default**
- `pal` - Emulate PAL machine timings (50Hz)
- `realtime` - Base time on real elapsed times - **default**
- `cycletime` - Base time on number of CPU cycles
- `fastmath` - Use Altirra fast math pack - **default**
- `atarimath` - Use original Atari math pack

Multiple options can be specified: `-o ntsc,fastmath`

### Usage Examples

```bash
# Full debugging session
./build/crucible -d -t trace.txt -l labels.txt program.xex

# Profile a program
./build/crucible -p profile.txt program.xex

# Run with ATR image and custom root path
./build/crucible -I disk.atr -R /data/atari program.xex

# PAL timing with original math pack
./build/crucible -o pal,atarimath program.xex
```

## Device Support

### E: Device (Editor/Screen)

- **Purpose**: Standard input/output
- **Input**: Reads from standard input (stdin)
- **Output**: Writes to standard output (stdout)
- **Translation**: ATASCII to/from ASCII (unless `-b` flag used)

### D: Device (Disk)

- **Purpose**: ATR disk image access
- **Format**: DOS 2.x, SpartaDOS, BW-DOS
- **Features**:
  - File reading from ATR images
  - Directory listing
  - Subdirectory navigation
  - File attribute support
- **Usage**: Load with `-I` option
- **Limitations**: Read-only (no file writing to ATR)

### H: Device (Host)

- **Purpose**: Host filesystem access
- **Features**:
  - File reading and writing
  - Directory listing
  - Full CIO compatibility
- **Root Path**: Set with `-R` option (defaults to current directory)
- **Example**: `H1:FILE.TXT` accesses `FILE.TXT` in root path

### K: Device (Keyboard)

- **Purpose**: Character input from console
- **Features**: Single character input with ATASCII translation

### R: Device (RS-232/Serial)

- **Purpose**: Serial I/O emulation
- **Features**:
  - Baud rate configuration
  - Translation mode support
  - Basic serial communication

## File Formats

### XEX Files

- **Format**: Atari executable format
- **Extension**: `.xex`
- **Structure**: Multiple segments with load addresses
- **Usage**: Primary format for running Atari programs

### ATR Files

- **Format**: Atari disk image format
- **Extension**: `.atr`
- **Structure**: Header + sector data
- **Sizes**: Up to 16MB supported (65535 sectors × 256 bytes)
- **Sector Sizes**: 128 or 256 bytes per sector
- **Usage**: Load with `-I` option

### Label Files

- **Format**: Text file with address labels
- **Purpose**: Makes trace output readable
- **Format**: `LABEL = $ADDRESS` or `LABEL = ADDRESS`
- **Example**:
  ```
  MAIN = $0600
  LOOP = $0620
  ```

## Examples

### Example 1: Simple Program Execution

```bash
# Run a hello world program
./build/crucible hello.xex
# Output: HELLO
```

### Example 2: Debugging with Trace

```bash
# Create a label file (labels.txt)
echo "MAIN = \$0600" > labels.txt
echo "LOOP = \$0620" >> labels.txt

# Run with trace
./build/crucible -d -t trace.txt -l labels.txt program.xex
```

### Example 3: Working with ATR Images

```bash
# Boot from ATR image
./build/crucible -I disk.atr

# Run program with ATR image loaded
./build/crucible -I disk.atr program.xex
```

### Example 4: Host Filesystem Access

```bash
# Set root path and run program
./build/crucible -R /home/user/atari_files program.xex

# Program can access files via H: device
# H1:FILE.TXT maps to /home/user/atari_files/FILE.TXT
```

### Example 5: Profiling

```bash
# Generate profile
./build/crucible -p profile.txt program.xex

# View profile (shows cycle counts per instruction)
cat profile.txt
```

## Troubleshooting

### Program Doesn't Run

- **Check file format**: Ensure file is valid XEX format
- **Check debug output**: Use `-d` flag to see what's happening
- **Check error level**: Use `-e full` to see all errors

### ATR Image Not Loading

- **Check file format**: Ensure ATR has valid header
- **Check file size**: Supported up to 16MB
- **Check sector size**: Must be 128 or 256 bytes

### File Access Issues

- **D: Device**: Requires ATR image loaded with `-I`
- **H: Device**: Check root path with `-R` option
- **Permissions**: Ensure files are readable/writable

### Debug Output Too Verbose

- **Use error levels**: `-e mem` or `-e none`
- **Filter output**: Redirect stderr to file
- **Use labels**: Makes trace output more readable

### Performance Issues

- **Use cycle-based timing**: `-o cycletime`
- **Disable DOS**: `-D` flag if not needed
- **Reduce debug output**: Remove `-d` flag

### Common Error Messages

- **"can't open disk image"**: Check file path and permissions
- **"invalid instruction"**: Program may have bugs or incompatible code
- **"channel not open"**: Program trying to access closed IOCB
- **"invalid command"**: Unsupported CIO command

### Getting Help

- Use `-h` flag for command-line help
- Check documentation in `docs/` folder
- Review test files in `testfiles/` for examples
- Enable debug output with `-d` flag

## Advanced Usage

### Custom I/O Handlers

The simulator supports custom character I/O handlers through the API (see API_REFERENCE.md for details).

### Label File Format

Labels can be specified in multiple formats:
- `LABEL = $1234` (hexadecimal)
- `LABEL = 1234` (decimal)
- `LABEL = 0x1234` (hexadecimal with 0x prefix)

### Profile Analysis

Profile files show:
- Cycle counts per instruction address
- Branch statistics
- Memory access patterns
- Performance hotspots

### Trace Analysis

Trace files show:
- Instruction execution sequence
- Register values
- Memory accesses
- Call stack information

## See Also

- [Architecture Documentation](ARCHITECTURE.md) - Technical details
- [API Reference](API_REFERENCE.md) - Developer documentation
- [Building Guide](BUILDING.md) - Build system details
- [Changelog](CHANGELOG.md) - Version history

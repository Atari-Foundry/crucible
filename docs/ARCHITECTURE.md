# Crucible Architecture Documentation

## Table of Contents

1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Core Components](#core-components)
4. [Memory Map](#memory-map)
5. [Device Handlers](#device-handlers)
6. [Filesystem Implementation](#filesystem-implementation)
7. [Hardware Emulation](#hardware-emulation)
8. [Callback System](#callback-system)
9. [Build System](#build-system)

## Overview

Crucible is a 6502 simulator with Atari 8-bit BIOS emulation. It consists of several layers:

1. **CPU Simulator** (`sim65`) - 6502 instruction execution
2. **BIOS Emulation** (`atari`, `atcio`) - Atari OS calls
3. **Device Handlers** - CIO device implementations
4. **Filesystem** - ATR and host filesystem access
5. **Hardware** - POKEY, GTIA, ANTIC, PIA emulation

## System Architecture

```
┌─────────────────────────────────────────┐
│         Application (XEX)               │
└──────────────┬──────────────────────────┘
               │
┌──────────────▼──────────────────────────┐
│      Atari BIOS Emulation               │
│  ┌──────────┐  ┌──────────┐            │
│  │   CIO    │  │   DOS    │            │
│  └────┬─────┘  └────┬─────┘            │
└───────┼─────────────┼──────────────────┘
        │             │
┌───────▼─────────────▼──────────────────┐
│      Device Handlers                   │
│  ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐       │
│  │ E:│ │ D:│ │ H:│ │ K:│ │ R:│       │
│  └───┘ └───┘ └───┘ └───┘ └───┘       │
└───────┬─────────────┬──────────────────┘
        │             │
┌───────▼─────────────▼──────────────────┐
│   Filesystem Layer                     │
│  ┌──────────┐  ┌──────────┐           │
│  │   ATR    │  │   Host   │           │
│  └──────────┘  └──────────┘           │
└────────────────────────────────────────┘
        │
┌───────▼────────────────────────────────┐
│      6502 CPU Simulator (sim65)        │
└────────────────────────────────────────┘
```

## Core Components

### sim65 - CPU Simulator

**Location**: `src/sim65.c`, `src/sim65.h`

**Purpose**: Core 6502 instruction execution engine

**Key Features**:
- Full 6502 instruction set
- Memory management with callbacks
- Cycle counting
- Profiling support
- Debug tracing

**Key Functions**:
- `sim65_new()` - Create simulator instance
- `sim65_run()` - Execute instructions
- `sim65_call()` - Call subroutine and return
- `sim65_add_callback()` - Register memory/execution callbacks

### atari - BIOS Emulation

**Location**: `src/atari.c`, `src/atari.h`

**Purpose**: Atari OS vector and BIOS emulation

**Key Features**:
- OS vector handling
- XEX file loading
- ROM loading
- Boot from disk image
- Math pack integration

**Key Functions**:
- `atari_init()` - Initialize BIOS
- `atari_xex_load()` - Load XEX file
- `atari_boot_image()` - Boot from ATR image

### atcio - CIO System

**Location**: `src/atcio.c`, `src/atcio.h`

**Purpose**: Central I/O (CIO) system implementation

**Key Features**:
- CIOV vector emulation
- IOCB management
- Device handler routing
- Error handling

**Key Functions**:
- `atari_cio_init()` - Initialize CIO system
- Device handler registration
- IOCB parameter handling

## Memory Map

### Standard Atari Memory Layout

```
$0000 - $00FF   Zero Page
$0100 - $01FF   Stack
$0200 - $02FF   OS RAM
$0300 - $03FF   IOCB and vectors
$0400 - $07FF   Free RAM
$0800 - $BFFF   User RAM (default)
$C000 - $CFFF   Cartridge ROM (optional)
$D000 - $D7FF   Hardware registers
$E000 - $FFFF   OS ROM / BIOS
```

### Hardware Registers

```
$D000 - $D01F   GTIA
$D200 - $D20F   POKEY
$D400 - $D40E   ANTIC
$D300 - $D3FF   PIA
```

### OS Vectors

```
$E456           CIOV (Central I/O Vector)
$E459           SIOV (Serial I/O Vector)
$E465           SETVBV (Set VBLANK Vector)
$E468           SYSVBV (System VBLANK Vector)
```

## Device Handlers

### E: Device (Editor/Screen)

**Handler**: `sim_SCREN()` in `src/atcio.c`

**Features**:
- Standard input/output
- ATASCII translation
- Screen positioning (PLOT, DRAWTO, FILLTO)
- Graphics mode support

### D: Device (Disk - ATR)

**Handler**: `sim_DISKD()` in `src/ataridos.c`

**Features**:
- ATR filesystem access
- Directory reading
- File reading
- Subdirectory navigation
- File attributes

**Implementation**: Uses `atrfs` module for filesystem operations

### H: Device (Host Filesystem)

**Handler**: `sim_HOST()` in `src/athost.c`

**Features**:
- Host filesystem access
- File read/write
- Directory reading
- Full CIO compatibility

**Root Path**: Configurable via `-R` option

### K: Device (Keyboard)

**Handler**: `sim_KEYBD()` in `src/atcio.c`

**Features**:
- Character input
- ATASCII translation
- Key code mapping

### R: Device (RS-232)

**Handler**: `sim_RDEV()` in `src/atrdev.c`

**Features**:
- Serial I/O emulation
- Baud rate configuration
- Translation modes

## Filesystem Implementation

### ATR Filesystem (atrfs)

**Location**: `src/atrfs.c`, `src/atrfs.h`

**Purpose**: Read files from ATR disk images

**Key Structures**:
```c
struct atr_file_handle {
    sim65 s;
    uint16_t start_sector;
    uint16_t current_sector;
    uint32_t file_size;
    uint32_t position;
    uint8_t sector_buffer[256];
    // ...
};

struct atr_dir_entry {
    uint8_t flags;
    uint16_t start_sector;
    uint16_t sector_count;
    uint32_t byte_count;
    char filename[12];
    char full_filename[13];
};
```

**Key Functions**:
- `atrfs_find_file()` - Find file in directory
- `atrfs_open_file()` - Open file handle
- `atrfs_read_byte()` - Read byte from file
- `atrfs_read_directory()` - Read directory entries

**Supported Formats**:
- DOS 2.x
- SpartaDOS 3.x
- BW-DOS

**Features**:
- VTOC reading
- Sector chain following
- Subdirectory navigation
- File attribute parsing

### Host Filesystem (athost)

**Location**: `src/athost.c`, `src/athost.h`

**Purpose**: Access host operating system filesystem

**Features**:
- Standard file operations
- Directory reading
- Path resolution
- Error handling

## Hardware Emulation

### POKEY

**Location**: `src/hw.c`

**Registers Emulated**:
- `$D200-$D209` - Audio and keyboard
- `$D20A-$D20F` - Serial I/O and interrupts
- `$D20D-$D20F` - Potentiometer inputs

**Features**:
- Random number generator
- Serial I/O
- Interrupt handling
- Audio registers (stubs)
- Keyboard scanning

### GTIA

**Location**: `src/hw.c`

**Registers Emulated**:
- `$D000-$D01F` - All GTIA registers (stubs)

**Features**:
- CONSOL register (console keys)
- Player/Missile graphics (stubs)
- Color registers (stubs)
- Priority registers (stubs)

### ANTIC

**Location**: `src/hw.c`

**Registers Emulated**:
- `$D400-$D40E` - All ANTIC registers (stubs)

**Features**:
- VCOUNT (vertical line counter)
- DMA control (stubs)
- Display list (stubs)
- Scroll registers (stubs)

### PIA

**Location**: `src/hw.c`

**Registers Emulated**:
- `$D300` - PORTB (memory banking)
- `$D302` - PORTA (joystick/paddle)
- `$D303` - PACTL
- `$D301` - PBCTL

**Features**:
- Memory banking control
- Joystick/paddle inputs (stubs)
- Control register support

## Callback System

### Execution Callbacks

Callbacks are registered at specific addresses and executed when the PC reaches that address.

**Example**:
```c
add_rts_callback(s, CIOV, 1, sim_CIOV);
```

This places an RTS instruction at `$E456` and registers `sim_CIOV` as a callback.

### Memory Callbacks

- **Read callbacks**: Executed when memory is read
- **Write callbacks**: Executed when memory is written
- **Execution callbacks**: Executed when instruction is fetched

### RTS Simulation

For callbacks that replace RTS instructions, the callback must simulate the RTS:

```c
// Pop return address from stack
regs->s = (regs->s + 1) & 0xFF;
val = peek(s, 0x100 + regs->s);
regs->pc = val;
regs->s = (regs->s + 1) & 0xFF;
val = peek(s, 0x100 + regs->s);
regs->pc |= val << 8;
regs->pc = (regs->pc + 1) & 0xFFFF;
```

## Build System

### Makefile Structure

**Targets**:
- `all` - Build project (default)
- `clean` - Remove build artifacts
- `help` - Show help
- `test` - Run tests

**Variables**:
- `CC` - C compiler (gcc)
- `CFLAGS` - Compiler flags
- `LDLIBS` - Linker libraries
- `BDIR` - Build directory
- `ODIR` - Object directory
- `TARGET` - Executable path

### Source Files

**Core**:
- `sim65.c` - CPU simulator
- `atari.c` - BIOS emulation
- `atcio.c` - CIO system
- `main.c` - Main program

**Devices**:
- `ataridos.c` - D: device
- `athost.c` - H: device
- `atrdev.c` - R: device
- `atsio.c` - SIO emulation

**Filesystem**:
- `atrfs.c` - ATR filesystem
- `dosfname.c` - DOS filename handling

**Hardware**:
- `hw.c` - Hardware registers

**Math**:
- `mathpack.c` - Math pack integration

### Dependencies

- Standard C library
- Math library (`-lm`)
- No external dependencies

### Compilation

```bash
# Debug build
make CFLAGS="-g -O0"

# Release build
make CFLAGS="-O3 -flto"

# Clean and rebuild
make clean && make
```

## Data Flow

### Program Execution

1. Load XEX file → Parse segments → Load into memory
2. Set RUNAD → Jump to entry point
3. Execute instructions → Handle callbacks
4. Return on RTS or error

### CIO Call Flow

1. Program calls CIOV (`JSR $E456`)
2. Callback `sim_CIOV` executes
3. Parse IOCB parameters
4. Route to device handler
5. Device handler processes request
6. Return to caller (RTS simulation)

### File Access Flow

**ATR (D:)**:
1. Parse filename
2. Search directory via `atrfs_find_file()`
3. Open file handle via `atrfs_open_file()`
4. Read sectors via `atrfs_read_byte()`
5. Follow sector chains

**Host (H:)**:
1. Parse filename
2. Resolve path (root + filename)
3. Open file via `fopen()`
4. Read/write via standard I/O

## Error Handling

### Error Levels

- `none` - No error reporting
- `mem` - Memory errors only
- `full` - All errors (default)

### Error Types

- `sim65_err_exec_undef` - Undefined instruction
- `sim65_err_exec_uninit` - Uninitialized memory execution
- `sim65_err_read_uninit` - Uninitialized memory read
- `sim65_err_write_rom` - Write to ROM
- `sim65_err_break` - BRK instruction
- `sim65_err_invalid_ins` - Invalid instruction

## Performance Considerations

### Cycle Counting

- Accurate cycle counting for timing
- Supports real-time and cycle-based timing
- Profiling for performance analysis

### Memory Management

- Static memory allocation
- No dynamic allocation during execution
- Efficient callback lookup

### Optimization

- Link-time optimization (LTO) enabled
- Aggressive compiler optimizations
- Inline functions for hot paths

## See Also

- [User Guide](USER_GUIDE.md) - Usage instructions
- [API Reference](API_REFERENCE.md) - Developer API
- [Building Guide](BUILDING.md) - Build details

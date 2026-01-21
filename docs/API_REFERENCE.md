# Crucible API Reference

## Table of Contents

1. [Overview](#overview)
2. [Core Simulator API (sim65)](#core-simulator-api-sim65)
3. [Atari BIOS API](#atari-bios-api)
4. [ATR Filesystem API](#atr-filesystem-api)
5. [Host Filesystem API](#host-filesystem-api)
6. [SIO API](#sio-api)
7. [Callback System](#callback-system)
8. [Data Structures](#data-structures)

## Overview

This document provides a complete reference for the crucible API. The API is organized into several modules:

- **sim65**: Core 6502 CPU simulator
- **atari**: Atari BIOS emulation
- **atrfs**: ATR filesystem access
- **athost**: Host filesystem device
- **atsio**: SIO and ATR image access

## Core Simulator API (sim65)

### Simulator Creation and Management

#### `sim65 sim65_new(void)`

Creates a new simulator instance.

**Returns**: Simulator handle, or NULL on error

**Example**:
```c
sim65 s = sim65_new();
if (!s) {
    fprintf(stderr, "Failed to create simulator\n");
    exit(1);
}
```

#### `void sim65_free(sim65 s)`

Frees a simulator instance and all associated memory.

**Parameters**:
- `s`: Simulator handle

### Memory Management

#### `void sim65_add_ram(sim65 s, unsigned addr, unsigned len)`

Adds an uninitialized RAM region.

**Parameters**:
- `s`: Simulator handle
- `addr`: Starting address
- `len`: Length in bytes

#### `void sim65_add_zeroed_ram(sim65 s, unsigned addr, unsigned len)`

Adds a zeroed RAM region.

**Parameters**:
- `s`: Simulator handle
- `addr`: Starting address
- `len`: Length in bytes

#### `void sim65_add_data_ram(sim65 s, unsigned addr, const unsigned char *data, unsigned len)`

Adds a RAM region initialized with data.

**Parameters**:
- `s`: Simulator handle
- `addr`: Starting address
- `data`: Initial data
- `len`: Length in bytes

#### `void sim65_add_data_rom(sim65 s, unsigned addr, const unsigned char *data, unsigned len)`

Adds a ROM region (read-only).

**Parameters**:
- `s`: Simulator handle
- `addr`: Starting address
- `data`: ROM data
- `len`: Length in bytes

#### `unsigned sim65_get_byte(sim65 s, unsigned addr)`

Reads a byte from memory.

**Parameters**:
- `s`: Simulator handle
- `addr`: Memory address

**Returns**: Byte value, or 0x100 if invalid/uninitialized

### Execution Control

#### `enum sim65_error sim65_run(sim65 s, struct sim65_reg *regs, unsigned addr)`

Runs the simulation from the given address.

**Parameters**:
- `s`: Simulator handle
- `regs`: Initial register values (NULL for zero)
- `addr`: Starting address

**Returns**: Error code (0 = success)

**Example**:
```c
struct sim65_reg regs = {0};
regs.pc = 0x0600;
enum sim65_error err = sim65_run(s, &regs, 0x0600);
```

#### `enum sim65_error sim65_call(sim65 s, struct sim65_reg *regs, unsigned addr)`

Calls a subroutine via JSR and returns on RTS.

**Parameters**:
- `s`: Simulator handle
- `regs`: Register values (NULL for zero)
- `addr`: Subroutine address

**Returns**: Error code (0 = success, `sim65_err_call_ret` = normal return)

### Callbacks

#### `void sim65_add_callback(sim65 s, unsigned addr, sim65_callback cb, enum sim65_cb_type type)`

Adds a callback at a specific address.

**Parameters**:
- `s`: Simulator handle
- `addr`: Address for callback
- `cb`: Callback function
- `type`: Callback type (read, write, or exec)

**Callback Types**:
- `sim65_cb_read`: Memory read callback
- `sim65_cb_write`: Memory write callback
- `sim65_cb_exec`: Instruction execution callback

#### `void sim65_add_callback_range(sim65 s, unsigned addr, unsigned len, sim65_callback cb, enum sim65_cb_type type)`

Adds callbacks for an address range.

**Parameters**:
- `s`: Simulator handle
- `addr`: Starting address
- `len`: Range length
- `cb`: Callback function
- `type`: Callback type

### Debugging and Tracing

#### `void sim65_set_debug(sim65 s, enum sim65_debug level)`

Sets debug output level.

**Parameters**:
- `s`: Simulator handle
- `level`: Debug level (`sim65_debug_none`, `sim65_debug_messages`, `sim65_debug_trace`)

#### `void sim65_set_trace_file(sim65 s, FILE *f)`

Sets trace output file.

**Parameters**:
- `s`: Simulator handle
- `f`: File handle (NULL for stderr)

#### `void sim65_set_error_level(sim65 s, enum sim65_error_lvl level)`

Sets error reporting level.

**Parameters**:
- `s`: Simulator handle
- `level`: Error level (`sim65_errlvl_none`, `sim65_errlvl_memory`, `sim65_errlvl_full`)

#### `int sim65_dprintf(sim65 s, const char *format, ...)`

Prints debug message (if debug enabled).

**Parameters**:
- `s`: Simulator handle
- `format`: printf-style format string
- `...`: Format arguments

#### `int sim65_eprintf(sim65 s, const char *format, ...)`

Prints error message (always).

**Parameters**:
- `s`: Simulator handle
- `format`: printf-style format string
- `...`: Format arguments

### Profiling

#### `void sim65_set_profiling(sim65 s, int enable)`

Enables or disables profiling.

**Parameters**:
- `s`: Simulator handle
- `enable`: Non-zero to enable

#### `struct sim65_profile sim65_get_profile_info(sim65 s)`

Gets profiling information.

**Returns**: Profile structure with cycle counts and statistics

### Utility Functions

#### `void sim65_set_flags(sim65 s, uint8_t flag, uint8_t val)`

Sets or clears processor flags.

**Parameters**:
- `s`: Simulator handle
- `flag`: Flag mask (SIM65_FLAG_C, SIM65_FLAG_Z, etc.)
- `val`: Value (0 = clear, non-zero = set)

#### `void sim65_set_cycle_limit(sim65 s, uint64_t limit)`

Sets cycle execution limit.

**Parameters**:
- `s`: Simulator handle
- `limit`: Maximum cycles (0 = unlimited)

#### `void sim65_print_reg(const sim65 s, FILE *f)`

Prints current register values.

**Parameters**:
- `s`: Simulator handle
- `f`: Output file

#### `const char *sim65_error_str(sim65 s, enum sim65_error e)`

Gets error message string.

**Parameters**:
- `s`: Simulator handle
- `e`: Error code

**Returns**: Error message string

## Atari BIOS API

### Initialization

#### `void atari_init(sim65 s, emu_options *opts)`

Initializes Atari BIOS emulation.

**Parameters**:
- `s`: Simulator handle
- `opts`: Emulation options (NULL for defaults)

**Options Structure**:
```c
typedef struct {
    int (*get_char)(void);  // Character input callback
    void (*put_char)(int);  // Character output callback
    int flags;              // Option flags
} emu_options;
```

**Option Flags**:
- `atari_opt_no_dos`: Skip DOS emulation
- `atari_opt_pal`: PAL timing (50Hz)
- `atari_opt_cycletime`: Cycle-based timing
- `atari_opt_atari_mathpack`: Use original Atari math pack

#### `int atari_add_option(emu_options *opt, const char *str)`

Parses option string and updates options.

**Parameters**:
- `opt`: Options structure
- `str`: Option string (e.g., "pal,fastmath")

**Returns**: 0 on success, non-zero on error

### File Loading

#### `enum sim65_error atari_xex_load(sim65 s, const char *name, int check)`

Loads and runs a XEX file.

**Parameters**:
- `s`: Simulator handle
- `name`: XEX filename
- `check`: Non-zero to validate format

**Returns**: Error code

#### `enum sim65_error atari_rom_load(sim65 s, int addr, const char *name)`

Loads a ROM file at the given address.

**Parameters**:
- `s`: Simulator handle
- `addr`: Load address
- `name`: ROM filename

**Returns**: Error code

#### `int atari_load_image(sim65 s, const char *file_name)`

Loads an ATR disk image.

**Parameters**:
- `s`: Simulator handle
- `file_name`: ATR filename

**Returns**: 0 on success, non-zero on error

#### `enum sim65_error atari_boot_image(sim65 s)`

Boots from a loaded disk image.

**Parameters**:
- `s`: Simulator handle

**Returns**: Error code

### DOS Configuration

#### `void atari_dos_set_root(sim65 s, const char *path)`

Sets root path for D: device.

**Parameters**:
- `s`: Simulator handle
- `path`: Root directory path (NULL for current directory)

#### `void atari_dos_add_cmdline(sim65 s, const char *cmd)`

Adds command-line argument to DOS.

**Parameters**:
- `s`: Simulator handle
- `cmd`: Command-line string

### Utility Functions

#### `void add_rts_callback(sim65 s, unsigned addr, unsigned len, sim65_callback cb)`

Installs callback with RTS instruction.

**Parameters**:
- `s`: Simulator handle
- `addr`: Callback address
- `len`: Range length
- `cb`: Callback function

#### `int atari_get_flags(sim65 s)`

Gets current emulation flags.

**Returns**: Flags value

## ATR Filesystem API

### File Operations

#### `int atrfs_find_file(sim65 s, const char *path, struct atr_dir_entry *entry)`

Finds a file in the ATR filesystem.

**Parameters**:
- `s`: Simulator handle
- `path`: File path (e.g., "FILE.EXT" or "DIR\FILE.EXT")
- `entry`: Output directory entry

**Returns**: 0 on success, -1 on error

**Example**:
```c
struct atr_dir_entry entry;
if (atrfs_find_file(s, "MYFILE.TXT", &entry) == 0) {
    printf("Found file: %s\n", entry.filename);
}
```

#### `struct atr_file_handle *atrfs_open_file(sim65 s, uint16_t start_sector, uint32_t size)`

Opens a file handle for reading.

**Parameters**:
- `s`: Simulator handle
- `start_sector`: Starting sector number
- `size`: File size in bytes

**Returns**: File handle, or NULL on error

#### `int atrfs_read_byte(struct atr_file_handle *fh)`

Reads a byte from file.

**Parameters**:
- `fh`: File handle

**Returns**: Byte value, or -1 on EOF/error

#### `int atrfs_seek(struct atr_file_handle *fh, uint32_t position)`

Seeks to position in file.

**Parameters**:
- `fh`: File handle
- `position`: Byte position

**Returns**: 0 on success, -1 on error

#### `void atrfs_close_file(struct atr_file_handle *fh)`

Closes and frees file handle.

**Parameters**:
- `fh`: File handle

### Directory Operations

#### `int atrfs_read_directory(sim65 s, uint16_t dir_sector, struct atr_dir_entry *entries, int max_entries)`

Reads directory entries.

**Parameters**:
- `s`: Simulator handle
- `dir_sector`: Directory sector number
- `entries`: Output array
- `max_entries`: Maximum entries to read

**Returns**: Number of entries read, or -1 on error

## Host Filesystem API

### Initialization

#### `void atari_host_init(sim65 s)`

Initializes H: device handler.

**Parameters**:
- `s`: Simulator handle

#### `void atari_host_set_root(sim65 s, const char *path)`

Sets root path for H: device.

**Parameters**:
- `s`: Simulator handle
- `path`: Root directory path (NULL for current directory)

**Example**:
```c
atari_host_init(s);
atari_host_set_root(s, "/home/user/atari_files");
```

## SIO API

### Initialization

#### `void atari_sio_init(sim65 s)`

Initializes SIO emulation.

**Parameters**:
- `s`: Simulator handle

### Disk Image Operations

#### `int atari_sio_load_image(sim65 s, const char *file_name)`

Loads an ATR disk image.

**Parameters**:
- `s`: Simulator handle
- `file_name`: ATR filename

**Returns**: 0 on success, non-zero on error

#### `enum sim65_error atari_sio_boot(sim65 s)`

Boots from loaded disk image.

**Parameters**:
- `s`: Simulator handle

**Returns**: Error code

### ATR Access

#### `const struct atr_disk_image *atari_sio_get_disk_image(sim65 s)`

Gets pointer to loaded disk image.

**Parameters**:
- `s`: Simulator handle

**Returns**: Disk image structure, or NULL if not loaded

#### `int atari_sio_read_sector(sim65 s, unsigned sector, uint8_t *buffer, unsigned *size)`

Reads a sector from ATR image.

**Parameters**:
- `s`: Simulator handle
- `sector`: Sector number (1-based)
- `buffer`: Output buffer
- `size`: Input/output buffer size

**Returns**: 0 on success, non-zero on error

## Callback System

### Callback Function Signature

```c
typedef int (*sim65_callback)(sim65 s, struct sim65_reg *regs, 
                               unsigned addr, int data);
```

**Parameters**:
- `s`: Simulator handle
- `regs`: Register values before instruction
- `addr`: Address causing callback
- `data`: 
  - For read callbacks: unused
  - For write callbacks: value being written
  - For exec callbacks: `sim65_cb_exec`

**Returns**:
- For read callbacks: byte value (0-255)
- For other callbacks: error code (0 = success, negative = error)

### Example Callback

```c
static int my_callback(sim65 s, struct sim65_reg *regs, 
                       unsigned addr, int data)
{
    if (data == sim65_cb_exec) {
        // Instruction execution callback
        sim65_dprintf(s, "Executing at $%04X\n", addr);
        // Simulate RTS if needed
        return 0;
    }
    return 0;
}

// Register callback
sim65_add_callback(s, 0xE456, my_callback, sim65_cb_exec);
```

## Data Structures

### `struct sim65_reg`

CPU register values.

```c
struct sim65_reg {
    uint16_t pc;  // Program counter
    uint8_t a;    // Accumulator
    uint8_t x;    // X register
    uint8_t y;    // Y register
    uint8_t p;    // Processor status
    uint8_t s;    // Stack pointer
};
```

### `struct atr_file_handle`

ATR file handle.

```c
struct atr_file_handle {
    uint16_t start_sector;
    uint16_t current_sector;
    uint32_t file_size;
    uint32_t position;
    uint8_t sector_buffer[256];
    int sector_pos;
    int sector_size;
    sim65 sim;
};
```

### `struct atr_dir_entry`

ATR directory entry.

```c
struct atr_dir_entry {
    uint8_t flags;
    uint16_t start_sector;
    uint16_t sector_count;
    uint32_t byte_count;
    char filename[12];
    uint8_t attributes;
    int is_hidden;
    int is_protected;
    int is_archived;
};
```

### `struct atr_disk_image`

ATR disk image structure.

```c
struct atr_disk_image {
    uint8_t *data;
    unsigned sec_size;
    unsigned sec_count;
};
```

### `enum sim65_error`

Error codes.

```c
enum sim65_error {
    sim65_err_none = 0,
    sim65_err_exec_undef = -1,
    sim65_err_exec_uninit = -2,
    sim65_err_read_undef = -3,
    sim65_err_read_uninit = -4,
    sim65_err_write_undef = -5,
    sim65_err_write_rom = -6,
    sim65_err_break = -7,
    sim65_err_invalid_ins = -8,
    sim65_err_call_ret = -9,
    sim65_err_cycle_limit = -10,
    sim65_err_user = -11
};
```

## Error Handling

All functions that can fail return error codes. Check return values:

```c
enum sim65_error err = atari_xex_load(s, "program.xex", 1);
if (err != sim65_err_none) {
    fprintf(stderr, "Error: %s\n", sim65_error_str(s, err));
}
```

## Memory Management

- Simulator handles memory allocation internally
- File handles must be closed with `atrfs_close_file()`
- Simulator instance must be freed with `sim65_free()`

## Thread Safety

The API is **not** thread-safe. Use one simulator instance per thread.

## See Also

- [User Guide](USER_GUIDE.md) - Usage examples
- [Architecture](ARCHITECTURE.md) - System design
- [Building Guide](BUILDING.md) - Build instructions

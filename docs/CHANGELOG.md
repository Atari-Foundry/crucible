# Changelog

All notable changes to crucible will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased] - 2026-01-21

### Added

- **ATR Filesystem Support**: Full ATR image filesystem access for D: device
  - Directory parsing for DOS 2.x, SpartaDOS, and BW-DOS formats
  - VTOC (Volume Table of Contents) reading
  - Sector chain following for file data access
  - Subdirectory navigation support
  - File attribute handling (locked, hidden, protected files)
  - Directory listing with SpartaDOS-style formatting
  - Support for ATR images up to 16MB (65535 sectors × 256 bytes)

- **H: Device**: New host filesystem device
  - Provides access to local host filesystem
  - Supports all standard file operations (OPEN, CLOSE, GET, PUT, POINT, NOTE)
  - Directory read support (mode 6)
  - Allows simultaneous access to ATR images (D:) and host files (H:)
  - Configurable root path via `-R` command-line option

- **Enhanced Hardware Emulation**:
  - **POKEY**: Added support for audio/timer registers
    - AUDF1-4 ($D200, $D202, $D204, $D206) - Audio frequency registers
    - AUDC1-4 ($D201, $D203, $D205, $D207) - Audio control registers
    - AUDCTL ($D208) - Audio control register
    - KBCODE ($D209) - Keyboard code register
    - POT0-7 ($D200-$D207 when reading) - Potentiometer inputs
  - **GTIA**: Added register stubs for all GTIA registers ($D000-$D01F)
    - Player/Missile graphics registers
    - Color registers
    - Priority and control registers
  - **ANTIC**: Added register stubs for all ANTIC registers ($D400-$D40E)
    - DMA control, display list, scroll registers
    - Light pen and NMI registers
  - **PIA**: Enhanced support
    - PORTA register for joystick/paddle inputs
    - Control register support (PACTL, PBCTL)

- **Build System Improvements**:
  - Added `make clean` target for cleaning build artifacts
  - Added `make help` target for usage information
  - Added `make test` target for automated testing

- **Documentation**:
  - Comprehensive documentation in `docs/` folder
  - User guide, architecture documentation, API reference
  - Build instructions and troubleshooting guides

### Changed

- **D: Device**: Now exclusively uses ATR filesystem when ATR image is loaded
  - Removed host filesystem fallback (use H: device instead)
  - Full directory parsing and file access from ATR images
  - Enhanced directory listing format with file attributes

- **H: Device Root Path**: Now configurable via `-R` command-line option
  - Previously defaulted to current directory (`.`)
  - Now uses same root path as D: device when `-R` is specified
  - Maintains backward compatibility with default behavior

- **Platform Compatibility**:
  - Improved cross-platform support for terminal input handling
  - Windows: Uses `CONIN$` for terminal input
  - Unix/Linux: Uses `/dev/tty` for terminal input

- **CIO System**:
  - Fixed RTS callback simulation in `sim_CIOV` for proper return address handling
  - Fixed stack preservation in `call_devtab` to prevent corruption from nested calls
  - All CIO operations now work correctly with proper return handling

### Fixed

- **CIO Return Handling**: Fixed critical bug where CIOV callbacks were not properly returning to caller
  - Implemented RTS simulation in `sim_CIOV` to properly handle return addresses
  - Fixed stack corruption in nested device handler calls
  - All test programs now execute successfully

- **Hardcoded Paths**: Removed hardcoded filesystem paths
  - Made `/dev/tty` platform-specific with proper Windows support
  - Made H: device root path configurable via command-line

### Technical Details
- New modules:
  - `src/atrfs.c` / `src/atrfs.h` - ATR filesystem implementation
  - `src/athost.c` / `src/athost.h` - H: device handler
- Modified modules:
  - `src/atsio.c` / `src/atsio.h` - Added ATR access API
  - `src/ataridos.c` - Integrated ATR filesystem
  - `src/atcio.c` - Registered H: device, fixed RTS callback handling
  - `src/hw.c` - Enhanced hardware register support
  - `src/ciodev.h` - Added H: device definitions
  - `src/main.c` - Added H: device root path configuration
  - `src/atari.c` - Improved cross-platform terminal handling

## Previous Versions

See git history for earlier changes from the original Mini65 project.

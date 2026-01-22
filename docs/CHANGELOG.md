# Changelog

All notable changes to crucible will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added - 2026-01-22 20:30:00

- **Contributing Guide**: Added `CONTRIBUTING.md` with guidelines for contributors
  - Development setup instructions
  - Coding standards and style guidelines
  - Testing requirements
  - Git workflow and pull request process

- **Release Process Improvements**:
  - Version numbers now included in release filenames (e.g., `crucible-1.0.0-linux-amd64`)
  - Automatic version detection and incrementing
  - Better error handling for existing releases
  - Release file verification before upload

- **macOS Cross-Compilation**:
  - Updated to use `macos-cross-compiler` Docker image
  - Simplified build process (no SDK setup required)
  - Support for both Intel (x86_64) and Apple Silicon (arm64) builds

- **Documentation**:
  - Added `docs/GIT_SETUP.md` - Git branch structure and workflow guide
  - Added `docs/MACOS_BUILD.md` - macOS build instructions
  - Updated CI/CD documentation for local release process

### Changed - 2026-01-22 20:30:00

- **Build System**:
  - macOS builds now use `ghcr.io/shepherdjerred/macos-cross-compiler` Docker image
  - Removed dependency on local macOS SDK files
  - `make release` now includes macOS builds by default

- **Git Workflow**:
  - Replaced personal branch name with generic `develop` branch
  - Updated all workflows and documentation to use `develop`/`main` structure
  - Auto-merge workflow now merges `develop` to `main`

- **Release Process**:
  - Releases are now created locally using `make github-release`
  - GitHub Actions release workflow disabled (builds done locally)
  - Improved version handling with automatic conflict detection

- **Project Structure**:
  - Enhanced `.gitignore` with editor and OS-specific patterns
  - Removed unused Docker build files (`Dockerfile.macos`, `build-macos.sh`)
  - Cleaned up tracked files (removed `.cursor/debug.log`)

- **Documentation**:
  - Updated README.md with quick links to docs and releases
  - Updated CI/CD documentation to reflect local release process
  - Updated workflow documentation

### Fixed - 2026-01-22 20:30:00

- **Compilation**: Fixed missing `athost.h` include in `main.c`
  - Resolved implicit function declaration error
  - All platforms now compile successfully

- **Release Process**:
  - Fixed issue where macOS binaries weren't included in releases
  - Fixed version consistency between tags and filenames
  - Improved error messages for release failures

### Added - 2026-01-21

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

### Changed - 2026-01-21

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

### Fixed - 2026-01-21

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

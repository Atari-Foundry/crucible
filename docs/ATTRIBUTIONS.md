# Attributions

This file documents the contributions and attributions for the crucible project.

## Project Rebranding

**crucible** - Small 6502 simulator with Atari 8bit bios
- Copyright (C) 2026 AtariFoundry.com
- Rebranded from Mini65 in 2026
- Enhanced with ATR filesystem support, H: device, and improved hardware emulation

## Original Project

**Mini65** - Small 6502 simulator with Atari 8bit bios
- Copyright (C) 2017-2024 Daniel Serpell
- Original implementation of the 6502 simulator and Atari 8-bit BIOS emulation
- Foundation for the crucible project

## Core Components

### Main Codebase
- **Author**: Daniel Serpell
- **Copyright**: (C) 2017-2024 Daniel Serpell
- **Components**: 
  - 6502 CPU simulator (sim65)
  - Atari BIOS emulation (atari, atcio, atsio, ataridos)
  - Hardware emulation (hw)
  - DOS file system emulation (dosfname)
  - Math pack integration (mathpack)
  - SIO disk device emulation
  - CIO device handlers (E:, K:, R:)
- **License**: GNU General Public License v2 or later

### ATR Filesystem Implementation
- **Author**: AtariFoundry.com
- **Copyright**: (C) 2026 AtariFoundry.com
- **Components**:
  - ATR filesystem access (atrfs)
  - Directory parsing for DOS 2.x, SpartaDOS, and BW-DOS formats
  - VTOC (Volume Table of Contents) reading
  - Sector chain following and file handle management
  - Subdirectory navigation
  - File attribute handling
- **License**: GNU General Public License v2 or later

### H: Device (Host Filesystem)
- **Author**: AtariFoundry.com
- **Copyright**: (C) 2026 AtariFoundry.com
- **Components**:
  - H: device handler (athost)
  - Host filesystem access for local file operations
  - Directory reading support
  - Full CIO compatibility (OPEN, CLOSE, GET, PUT, POINT, NOTE)
- **License**: GNU General Public License v2 or later

### Enhanced Hardware Emulation
- **Author**: AtariFoundry.com
- **Copyright**: (C) 2026 AtariFoundry.com
- **Components**:
  - Enhanced POKEY register support (audio, timers, keyboard)
  - GTIA register stubs (all registers $D000-$D01F)
  - ANTIC register stubs (all registers $D400-$D40E)
  - Enhanced PIA support (PORTA, control registers)
- **License**: GNU General Public License v2 or later

### CIO System Improvements
- **Author**: AtariFoundry.com
- **Copyright**: (C) 2026 AtariFoundry.com
- **Components**:
  - RTS callback simulation fixes
  - Stack preservation in nested calls
  - Improved callback return handling
- **License**: GNU General Public License v2 or later

### Math Pack - Altirra
- **File**: `asm/mathpack.s`
- **Author**: Avery Lee
- **Copyright**: (C) 2008-2016 Avery Lee
- **Source**: Altirra - Atari 800/800XL/5200 emulator
- **Component**: Modular Kernel ROM - Decimal Floating-Point Math Pack
- **License**: Permissive (copying and distribution permitted with copyright notice preserved)
- **Note**: Used for maximum compatibility with Atari software

### Original Atari OS Code
- **File**: `asm/bb1r2-mathpack.s`
- **Copyright**: 1984 ATARI
- **Component**: Original Atari Operating System code
- **Note**: Original copyright notice preserved as required

### Floating Point Package
- **File**: `asm/bb1r2-mathpack.s`
- **Author**: Shepardson Microsystems, Inc.
- **Copyright**: (C) 1978 Shepardson Microsystems, Inc.
- **Component**: Floating Point Package
- **Note**: Original copyright notice preserved as required

## License

This project is distributed under the GNU General Public License v2 or later.
See the LICENSE file for full details.

All original copyrights are preserved in accordance with the GPL and other applicable licenses.

## Acknowledgments

- **Daniel Serpell** for the original Mini65 project and 6502 simulator
- **Avery Lee** for the Altirra emulator and fast math pack
- **AtariFoundry.com** for enhancements and continued development

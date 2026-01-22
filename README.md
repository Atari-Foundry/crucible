# Crucible

Small 6502 simulator with Atari 8-bit BIOS emulation for debugging and running command-line Atari programs.

## Quick Links

- 📚 **[Documentation](docs/)** - Complete documentation and guides
- 🚀 **[Releases](https://github.com/Atari-Foundry/crucible/releases)** - Download pre-built binaries

## Quick Start

```bash
make
./build/crucible program.xex
```

## Documentation

**📚 [Complete documentation is available in `docs/`](docs/)**

- **[User Guide](docs/USER_GUIDE.md)** - Installation, usage, examples
- **[API Reference](docs/API_REFERENCE.md)** - Developer API
- **[Architecture](docs/ARCHITECTURE.md)** - Technical design
- **[Building Guide](docs/BUILDING.md)** - Build instructions
- **[Changelog](docs/CHANGELOG.md)** - Version history
- **[Attributions](docs/ATTRIBUTIONS.md)** - Credits

## Features

- 6502 CPU emulation with full instruction set
- Atari BIOS emulation (CIO, DOS, SIO)
- ATR filesystem access (DOS 2.x, SpartaDOS, BW-DOS)
- Host filesystem access via H: device
- Hardware register emulation (POKEY, GTIA, ANTIC, PIA)
- Debugging tools (tracing, profiling, labels)
- Cross-platform (Linux, macOS, Windows)

## License

GNU General Public License v2 or later. See [LICENSE](LICENSE) for details.


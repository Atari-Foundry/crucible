# Crucible Documentation

Welcome to the crucible documentation. This directory contains comprehensive documentation for the crucible 6502 simulator with Atari 8-bit BIOS emulation.

## Documentation Index

### User Documentation

- **[User Guide](USER_GUIDE.md)** - Complete user guide with examples
  - Installation instructions
  - Command-line options
  - Device support
  - File formats
  - Troubleshooting

### Developer Documentation

- **[Architecture](ARCHITECTURE.md)** - Technical architecture and design
  - System architecture overview
  - Core components
  - Memory map
  - Device handlers
  - Filesystem implementation
  - Hardware emulation
  - Callback system

- **[API Reference](API_REFERENCE.md)** - Complete API documentation
  - Core simulator API
  - Atari BIOS API
  - ATR filesystem API
  - Host filesystem API
  - SIO API
  - Data structures
  - Code examples

- **[Building Guide](BUILDING.md)** - Build system documentation
  - Prerequisites
  - Build options
  - Platform-specific instructions
  - Troubleshooting
  - Advanced configuration

### Project Information

- **[Changelog](CHANGELOG.md)** - Version history and changes
- **[Attributions](ATTRIBUTIONS.md)** - Credits and attributions

## Quick Start

1. **Build the project**:
   ```bash
   make
   ```

2. **Run a program**:
   ```bash
   ./build/crucible program.xex
   ```

3. **Get help**:
   ```bash
   ./build/crucible -h
   ```

## Documentation Structure

```
docs/
├── README.md           # This file
├── USER_GUIDE.md       # User documentation
├── ARCHITECTURE.md     # Technical architecture
├── API_REFERENCE.md    # API documentation
├── BUILDING.md         # Build instructions
├── CHANGELOG.md        # Version history
└── ATTRIBUTIONS.md     # Credits
```

## Getting Help

- **User Questions**: See [User Guide](USER_GUIDE.md)
- **Development Questions**: See [API Reference](API_REFERENCE.md)
- **Build Issues**: See [Building Guide](BUILDING.md)
- **Architecture Questions**: See [Architecture](ARCHITECTURE.md)

## Contributing

When contributing to crucible:

1. Read the [Architecture](ARCHITECTURE.md) documentation
2. Follow the coding style
3. Update [CHANGELOG.md](CHANGELOG.md) with changes
4. Update [ATTRIBUTIONS.md](ATTRIBUTIONS.md) if needed
5. Update relevant documentation

## License

See the main LICENSE file in the project root. All documentation is provided under the same license as the project (GPL v2 or later).

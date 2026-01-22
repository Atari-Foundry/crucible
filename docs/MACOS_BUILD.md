# Building macOS Binaries

This document explains how to build macOS binaries (Intel and Apple Silicon) using Docker.

## Prerequisites

1. **Docker** - Install Docker on your system
2. **macOS SDK** - Required for cross-compilation

## Getting the macOS SDK

The macOS SDK is required for cross-compilation. You have two options:

### Option 1: Download Pre-built SDK (Recommended)

Download from the community-maintained repository:

```bash
# Download macOS SDK (choose appropriate version)
wget https://github.com/phracker/MacOSX-SDKs/releases/download/11.3/MacOSX11.3.sdk.tar.xz

# Rename to the expected name
mv MacOSX11.3.sdk.tar.xz MacOSX.sdk.tar.xz
```

### Option 2: Extract from Xcode

If you have Xcode installed on macOS:

```bash
# On macOS, extract the SDK
cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs
tar -czf MacOSX.sdk.tar.xz MacOSX.sdk
```

**Note**: The macOS SDK is subject to Apple's licensing terms. Ensure you comply with Apple's license agreement.

## Building macOS Binaries

### Build Both Architectures

```bash
# Build both Intel (x86_64) and Apple Silicon (arm64)
make release-macos
```

### Build Individual Architectures

```bash
# Build only Intel (x86_64)
make release-macos-x86_64

# Build only Apple Silicon (arm64)  
make release-macos-arm64
```

The binaries will be created in the `release/` directory:
- `crucible-macos-x86_64` - Intel Macs
- `crucible-macos-arm64` - Apple Silicon Macs

## Using Docker Directly

You can also use Docker directly:

```bash
# Build the Docker image
docker build -f Dockerfile.macos -t crucible-macos-builder .

# Run the build
docker run --rm \
  -v $(pwd)/release:/workspace/release \
  -v $(pwd)/MacOSX.sdk.tar.xz:/workspace/MacOSX.sdk.tar.xz:ro \
  crucible-macos-builder
```

## Troubleshooting

### SDK Not Found

If you see "macOS SDK not found", ensure:
1. The SDK file is named `MacOSX.sdk.tar.xz` in the project root, OR
2. Set the `MACOS_SDK_PATH` environment variable:
   ```bash
   export MACOS_SDK_PATH=/path/to/MacOSX.sdk.tar.xz
   make release-macos
   ```

### Docker Build Fails

- Ensure Docker is running: `docker ps`
- Check Docker has enough resources (memory, disk space)
- Try rebuilding the image: `docker build --no-cache -f Dockerfile.macos -t crucible-macos-builder .`

### Compiler Not Found

If osxcross toolchain build fails:
- The SDK may be incompatible
- Try a different SDK version
- Check Docker logs for specific errors

## Integration with Release Process

macOS builds are optional and can be included in the full release:

```bash
# Build all platforms including macOS
make release release-macos

# Create GitHub release with all binaries (including macOS if built)
make github-release
```

The `github-release` target will automatically include macOS binaries if they exist in the `release/` directory.

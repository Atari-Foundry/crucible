# GitHub Actions Workflows

This directory contains GitHub Actions workflows for automated CI/CD.

## Workflows

### `ci.yml` - Continuous Integration
- **Triggers**: Push to `main`/`develop`, Pull Requests
- **Purpose**: Build and test on multiple platforms (Linux, Windows, macOS)
- **Actions**:
  - Builds with GCC and Clang on Linux
  - Builds on Windows with MinGW
  - Builds on macOS
  - Runs test suite
  - Verifies executable works

### `release.yml` - Release Creation (Disabled)
- **Status**: ⚠️ Disabled - Releases are created locally
- **Purpose**: This workflow is disabled. Releases are built locally and uploaded using `make github-release`
- **Local Release Process**:
  1. Build all platforms: `make release`
  2. Create GitHub release: `make github-release [VERSION=v1.2.3]`
- **See**: [CONTRIBUTING.md](../../CONTRIBUTING.md) for release instructions

### `auto-merge.yml` - Auto-Merge develop to main
- **Triggers**: After successful CI build on `develop` branch
- **Purpose**: Automatically merge `develop` branch to `main` after successful build
- **Actions**:
  - Waits for CI to complete successfully
  - Merges `develop` to `main`
  - Triggers release workflow on main
- **Note**: Requires branch protection and workflow permissions to be configured

### `merge-develop.yml` - Alternative Merge Workflow
- **Triggers**: Push to `develop` branch
- **Purpose**: Alternative approach that waits for CI then merges
- **Actions**:
  - Waits for CI workflow to complete
  - Merges `develop` to `main` if CI succeeds

## Usage

### Creating Releases

Releases are created **locally** using the Makefile:

```bash
# Build all platforms
make release

# Create GitHub release (auto-detects version or specify with VERSION=v1.2.3)
make github-release
```

The `make github-release` command will:
- Build binaries for Linux (amd64, arm64), Windows (x86_64), and macOS (x86_64, arm64)
- Rename files to include version number
- Generate release notes from CHANGELOG.md
- Create a GitHub release with all binaries

**Prerequisites**:
- Docker (for macOS builds)
- GitHub CLI (`gh`) installed and authenticated
- Cross-compilation tools (or Docker for macOS)

## Permissions

The workflows use `GITHUB_TOKEN` which is automatically provided by GitHub Actions. For releases, ensure the repository has write permissions enabled in Settings → Actions → General → Workflow permissions.

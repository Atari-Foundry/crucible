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

### `release.yml` - Release Creation
- **Triggers**: 
  - Push to `main` branch
  - Push of version tags (v*)
  - Manual workflow dispatch
- **Purpose**: Create GitHub releases with build artifacts
- **Actions**:
  - Builds for Linux, Windows, macOS
  - Creates release artifacts
  - Generates release notes from CHANGELOG
  - Creates GitHub release

### `auto-merge.yml` - Auto-Merge to Main
- **Triggers**: After successful CI build on `develop`, `feature/*`, or `fix/*` branches
- **Purpose**: Automatically merge to main after successful build
- **Actions**:
  - Merges branch to main
  - Triggers release workflow
- **Note**: Requires branch protection and workflow permissions to be configured

## Usage

### Automatic Releases
When code is pushed to `main`, a release is automatically created.

### Manual Releases
1. Go to Actions → Release → Run workflow
2. Enter version (e.g., `v1.0.0`)
3. Workflow will build and create release

### Tagged Releases
Push a tag to trigger a full release:
```bash
git tag v1.0.0
git push origin v1.0.0
```

## Permissions

The workflows use `GITHUB_TOKEN` which is automatically provided by GitHub Actions. For releases, ensure the repository has write permissions enabled in Settings → Actions → General → Workflow permissions.

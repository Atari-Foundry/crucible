# CI/CD Setup

This document describes the automated CI/CD pipeline for crucible.

## Overview

The project uses GitHub Actions for continuous integration and deployment. The workflow is:

1. **Build & Test** - On every push/PR, build and test on multiple platforms
2. **Auto-Merge** - (Optional) Automatically merge to main after successful build
3. **Release** - Automatically create GitHub release when code is merged to main

## Workflows

### CI Workflow (`ci.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`

**Actions:**
- Builds on Linux with GCC and Clang
- Builds on Windows with MinGW
- Builds on macOS
- Runs test suite (`make test`)
- Verifies executable works

**Status:** Build status is shown on PRs and commits.

### Release Workflow (`release.yml`)

**Triggers:**
- Push to `main` branch (automatic release)
- Push of version tags (e.g., `v1.0.0`)
- Manual workflow dispatch

**Actions:**
- Builds for Linux, Windows, macOS
- Creates release artifacts
- Generates release notes from CHANGELOG
- Creates GitHub release with binaries

**Versioning:**
- Automatic: `vYYYY.MM.DD-<commit-hash>` (e.g., `v2026.01.21-a1b2c3d`)
- Tagged: Uses tag name (e.g., `v1.0.0`)

### Auto-Merge Workflow (`auto-merge.yml`)

**Triggers:**
- After successful CI build on `develop`, `feature/*`, or `fix/*` branches

**Actions:**
- Automatically merges branch to `main`
- Triggers release workflow

**Note:** This requires proper branch protection settings in GitHub.

## Setup Instructions

### 1. Enable GitHub Actions

GitHub Actions are enabled by default. Ensure workflows are in `.github/workflows/`.

### 2. Configure Branch Protection (Optional)

For auto-merge to work:

1. Go to Settings → Branches
2. Add rule for `main` branch:
   - Require pull request reviews (optional)
   - Require status checks to pass
   - Require branches to be up to date
   - Include administrators

### 3. Enable Workflow Permissions

1. Go to Settings → Actions → General
2. Under "Workflow permissions":
   - Select "Read and write permissions"
   - Check "Allow GitHub Actions to create and approve pull requests"

### 4. Test the Pipeline

```bash
# Make a change
git checkout -b test-ci
# ... make changes ...
git commit -am "Test CI"
git push origin test-ci

# Create PR or push to develop to trigger CI
# After successful build, it will auto-merge to main (if configured)
# Then release workflow will create a release
```

## Manual Release

To create a manual release:

1. Go to Actions → Release
2. Click "Run workflow"
3. Enter version (e.g., `v1.0.0`)
4. Click "Run workflow"

Or push a tag:

```bash
git tag v1.0.0
git push origin v1.0.0
```

## Release Artifacts

Each release includes:
- `crucible-linux-x86_64` - Linux binary
- `crucible-windows-x86_64.exe` - Windows binary
- `crucible-macos-x86_64` - macOS binary

## Troubleshooting

### Build Fails

- Check Actions tab for error logs
- Verify all dependencies are available
- Check compiler compatibility

### Auto-Merge Not Working

- Verify branch protection is configured
- Check workflow permissions
- Ensure CI workflow completes successfully

### Release Not Created

- Verify push was to `main` branch
- Check workflow permissions
- Review release workflow logs

## Customization

### Change Release Triggers

Edit `.github/workflows/release.yml`:

```yaml
on:
  push:
    branches:
      - main
      - release/*  # Add custom branches
```

### Disable Auto-Merge

Delete or disable `.github/workflows/auto-merge.yml` if you prefer manual merging.

### Custom Build Matrix

Edit `.github/workflows/ci.yml` to add more platforms or compilers.

## See Also

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Building Guide](BUILDING.md) - Local build instructions

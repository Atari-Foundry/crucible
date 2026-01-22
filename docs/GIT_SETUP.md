# Git Branch Setup

This document describes the git branch structure and CI/CD workflow for crucible.

## Branch Structure

- **`develop`** - Development branch (your working branch)
- **`main`** - Production/release branch

## Workflow

1. **Development**: Work on `develop` branch
2. **CI**: Push to `develop` triggers CI build and tests
3. **Auto-Merge**: After successful CI, `develop` is automatically merged to `main`
4. **Release**: Releases are created locally using `make github-release`

## Initial Setup

### Create main branch

If `main` doesn't exist yet:

```bash
# Create main branch from current branch
git checkout -b main
git push origin main

# Or if main exists remotely
git fetch origin
git checkout main
git pull origin main
```

### Set up develop branch

```bash
# If you're currently on develop
git checkout develop

# Or create it from main
git checkout -b develop
git push origin develop
```

## CI/CD Flow

```
develop branch
    ↓ (push)
CI workflow runs (build & test)
    ↓ (success)
Auto-merge workflow merges to main
    ↓
main branch updated
    ↓
Release created locally: make github-release
```

## Manual Operations

### Merge manually (if needed)

```bash
git checkout main
git merge develop
git push origin main
```

### Create a release tag

```bash
git tag v1.0.0
git push origin v1.0.0
```

## Branch Protection (Recommended)

Set up branch protection for `main`:

1. Go to Settings → Branches
2. Add rule for `main`:
   - Require pull request reviews (optional)
   - Require status checks to pass before merging
   - Require branches to be up to date
   - Include administrators

## Workflow Permissions

Ensure workflows can push to main:

1. Go to Settings → Actions → General
2. Under "Workflow permissions":
   - Select "Read and write permissions"
   - Check "Allow GitHub Actions to create and approve pull requests"

## Troubleshooting

### Auto-merge not working

- Check that CI workflow completes successfully
- Verify workflow permissions are set correctly
- Check Actions tab for error messages

### Merge conflicts

If there are conflicts, resolve manually:

```bash
git checkout main
git merge develop
# Resolve conflicts
git commit
git push origin main
```

## See Also

- [CI/CD Documentation](CI_CD.md) - Complete CI/CD setup guide
- [GitHub Actions Workflows](../.github/workflows/README.md) - Workflow details

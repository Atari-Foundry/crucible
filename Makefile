#
#  crucible - Small 6502 simulator with Atari 8bit bios.
#  Copyright (C) 2026 AtariFoundry.com
#  Copyright (C) 2017-2019 Daniel Serpell
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program.  If not, see <http://www.gnu.org/licenses/>
#

CC=gcc
INCLUDES=-Iccan
CFLAGS=$(INCLUDES) -O3 -Wall -g -flto=auto
LDLIBS=-lm

BDIR=build
ODIR=$(BDIR)/obj
# Determine executable extension based on compiler
TARGET_EXT := $(if $(findstring mingw,$(CC)),.exe,)
TARGET=$(BDIR)/crucible$(TARGET_EXT)

RELEASE_DIR=release
.PHONY: all clean help test release release-docker release-linux-amd64 release-linux-arm64 release-windows-x86_64 release-macos-x86_64 release-macos-arm64 release-macos github-release

all: $(TARGET)

help:
	@echo "crucible - Small 6502 simulator with Atari 8bit bios"
	@echo ""
	@echo "Available targets:"
	@echo "  all                  - Build the crucible executable (default)"
	@echo "  clean                - Remove all build artifacts and test files"
	@echo "  test                 - Run test programs"
	@echo "  release              - Build release binaries for all platforms"
	@echo "  release-docker       - Build releases using Docker (alternative)"
	@echo "  release-linux-amd64  - Build Linux amd64 binary"
	@echo "  release-linux-arm64  - Build Linux arm64 binary"
	@echo "  release-windows-x86_64 - Build Windows x86_64 binary"
	@echo "  release-macos        - Build macOS binaries (Intel + Apple Silicon) using Docker"
	@echo "  release-macos-x86_64 - Build macOS Intel binary using Docker"
	@echo "  release-macos-arm64  - Build macOS Apple Silicon binary using Docker"
	@echo "  github-release       - Build and create GitHub release (requires gh CLI)"
	@echo "  help                 - Show this help message"
	@echo ""
	@echo "Build output: $(TARGET)"
	@echo "Object files: $(ODIR)/"
	@echo "Release output: $(RELEASE_DIR)/"

test: $(TARGET)
	@echo "Running crucible tests..."
	@echo ""
	@echo "Test 1: ATR image loading..."
	@$(TARGET) -I testfiles/test.atr -d 2>&1 | grep -q "loaded.*test.atr\|sectors of" && echo "  ✓ ATR image loads successfully" || echo "  ✗ ATR image load failed"
	@echo ""
	@echo "Test 2: XEX program loading (hello.xex)..."
	@$(TARGET) testfiles/hello.xex 2>&1 | grep -q "HELLO" && echo "  ✓ Program runs successfully (prints HELLO)" || echo "  ✗ Program failed"
	@echo ""
	@echo "Test 3: H: device test (read_h.xex)..."
	@$(TARGET) -R testfiles testfiles/read_h.xex 2>&1 | grep -q "SCREEN:" && echo "  ✓ Program runs successfully (H: device access works)" || echo "  ⚠ Program loads but encounters runtime issues"
	@echo ""
	@echo "Tests completed. See testfiles/TEST_RESULTS.md for detailed results."

SRC=\
 src/atari.c\
 src/atcio.c\
 src/ataridos.c\
 src/atrdev.c\
 src/atrfs.c\
 src/athost.c\
 src/atsio.c\
 src/dosfname.c\
 src/hw.c\
 src/main.c\
 src/mathpack.c\
 src/sim65.c\

OBJS=$(SRC:src/%.c=$(ODIR)/%.o)

$(TARGET): $(OBJS) | $(BDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -rf $(BDIR)
	rm -rf $(RELEASE_DIR)
	rm -f testfiles/*.xex testfiles/*.atr testfiles/TEST.TXT

# Release builds
release: release-linux-amd64 release-linux-arm64 release-windows-x86_64 release-macos
	@echo ""
	@echo "Release builds completed:"
	@ls -lh $(RELEASE_DIR)/

# macOS builds (require Docker and macOS SDK)
release-macos: release-macos-x86_64 release-macos-arm64
	@echo ""
	@echo "macOS builds completed:"
	@ls -lh $(RELEASE_DIR)/crucible-macos-* 2>/dev/null || echo "  No macOS binaries found"

release-macos-x86_64:
	@echo "Building macOS Intel (x86_64) using Docker..."
	@if ! command -v docker >/dev/null 2>&1; then \
		echo "  ✗ Docker not found. Please install Docker."; \
		exit 1; \
	fi
	@mkdir -p $(RELEASE_DIR)
	@echo "  Using macos-cross-compiler Docker image..."
	@docker run --platform=linux/amd64 --rm \
		-v $(CURDIR):/workspace \
		-v $(CURDIR)/$(RELEASE_DIR):/workspace/release \
		-w /workspace \
		ghcr.io/shepherdjerred/macos-cross-compiler:latest \
		sh -c " \
			rm -rf build-macos-x86_64 && \
			make CC=x86_64-apple-darwin24-gcc \
			     CFLAGS=\"-Iccan -O3 -Wall -flto=auto\" \
			     LDLIBS=\"-lm\" \
			     BDIR=build-macos-x86_64 \
			     TARGET=build-macos-x86_64/crucible && \
			cp build-macos-x86_64/crucible release/crucible-macos-x86_64 && \
			chmod +x release/crucible-macos-x86_64 && \
			echo '  ✓ macOS Intel build complete' \
		" || echo "  ⚠ macOS Intel build may have failed. Check Docker output above."

release-macos-arm64:
	@echo "Building macOS Apple Silicon (arm64) using Docker..."
	@if ! command -v docker >/dev/null 2>&1; then \
		echo "  ✗ Docker not found. Please install Docker."; \
		exit 1; \
	fi
	@mkdir -p $(RELEASE_DIR)
	@echo "  Using macos-cross-compiler Docker image..."
	@docker run --platform=linux/amd64 --rm \
		-v $(CURDIR):/workspace \
		-v $(CURDIR)/$(RELEASE_DIR):/workspace/release \
		-w /workspace \
		ghcr.io/shepherdjerred/macos-cross-compiler:latest \
		sh -c " \
			rm -rf build-macos-arm64 && \
			make CC=aarch64-apple-darwin24-gcc \
			     CFLAGS=\"-Iccan -O3 -Wall -flto=auto\" \
			     LDLIBS=\"-lm\" \
			     BDIR=build-macos-arm64 \
			     TARGET=build-macos-arm64/crucible && \
			cp build-macos-arm64/crucible release/crucible-macos-arm64 && \
			chmod +x release/crucible-macos-arm64 && \
			echo '  ✓ macOS Apple Silicon build complete' \
		" || echo "  ⚠ macOS Apple Silicon build may have failed. Check Docker output above."

# Alternative: Build releases using Docker (if cross-compilers are not available)
release-docker:
	@mkdir -p $(RELEASE_DIR)
	@echo "Building releases using Docker..."
	@docker build -f Dockerfile.release -t crucible-builder .
	@docker run --rm -v $(CURDIR)/$(RELEASE_DIR):/workspace/release crucible-builder
	@echo ""
	@echo "Docker release builds completed:"
	@ls -lh $(RELEASE_DIR)/

# Create GitHub release with all binaries
# Usage: make github-release [VERSION=v1.2.3] [SKIP_GIT=1] [AUTO_COMMIT=1]
# SKIP_GIT=1 skips git commit/push/merge steps
# AUTO_COMMIT=1 automatically commits changes without prompting
github-release:
	@echo ""; \
	echo "=== Preparing for release ==="; \
	CURRENT_BRANCH=$$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown"); \
	echo "Current branch: $$CURRENT_BRANCH"; \
	if [ "$(SKIP_GIT)" != "1" ]; then \
		echo ""; \
		echo "Step 1: Checking git status..."; \
		if [ -n "$$(git status --porcelain 2>/dev/null)" ]; then \
			echo "  Found uncommitted changes:"; \
			git status --short; \
			if [ "$(AUTO_COMMIT)" = "1" ]; then \
				COMMIT_MSG="Prepare release: $$(date '+%Y-%m-%d %H:%M:%S')"; \
				git add -A; \
				git commit -m "$$COMMIT_MSG" || { \
					echo "  ⚠ Failed to commit changes"; \
					exit 1; \
				}; \
				echo "  ✓ Changes committed automatically"; \
			else \
				read -p "  Commit these changes? (y/n) " -n 1 -r; \
				echo; \
				if [ "$$REPLY" = "y" ] || [ "$$REPLY" = "Y" ]; then \
					COMMIT_MSG="Prepare release: $$(date '+%Y-%m-%d %H:%M:%S')"; \
					git add -A; \
					git commit -m "$$COMMIT_MSG" || { \
						echo "  ⚠ Failed to commit changes"; \
						exit 1; \
					}; \
					echo "  ✓ Changes committed"; \
				else \
					echo "  ⚠ Skipping commit. Uncommitted changes remain."; \
				fi; \
			fi; \
		else \
			echo "  ✓ Working directory clean"; \
		fi; \
		echo ""; \
		echo "Step 2: Pushing to remote..."; \
		if git rev-parse --verify origin/$$CURRENT_BRANCH >/dev/null 2>&1; then \
			git push origin $$CURRENT_BRANCH || { \
				echo "  ⚠ Failed to push to origin/$$CURRENT_BRANCH"; \
				echo "     You may need to push manually: git push origin $$CURRENT_BRANCH"; \
				read -p "  Continue anyway? (y/n) " -n 1 -r; \
				echo; \
				if [ "$$REPLY" != "y" ] && [ "$$REPLY" != "Y" ]; then \
					exit 1; \
				fi; \
			}; \
		else \
			echo "  Creating and pushing branch $$CURRENT_BRANCH..."; \
			git push -u origin $$CURRENT_BRANCH || { \
				echo "  ⚠ Failed to push branch"; \
				exit 1; \
			}; \
		fi; \
		echo "  ✓ Pushed to origin/$$CURRENT_BRANCH"; \
		echo ""; \
		if [ "$$CURRENT_BRANCH" = "develop" ]; then \
			echo "Step 3: Merging to main..."; \
			echo "  Checking if auto-merge workflow is available..."; \
			CI_RUN_ID=$$(gh run list --workflow=ci.yml --branch=develop --limit=1 --json databaseId,status,conclusion -q '.[0].databaseId' 2>/dev/null); \
			if [ -n "$$CI_RUN_ID" ]; then \
				echo "  Waiting for CI to complete..."; \
				gh run watch $$CI_RUN_ID --exit-status || { \
					echo "  ⚠ CI failed. Do you want to merge to main anyway?"; \
					read -p "  Continue? (y/n) " -n 1 -r; \
					echo; \
					if [ "$$REPLY" != "y" ] && [ "$$REPLY" != "Y" ]; then \
						exit 1; \
					fi; \
				}; \
			fi; \
			echo "  Merging develop to main..."; \
			git fetch origin main:main 2>/dev/null || true; \
			git checkout main 2>/dev/null || { \
				echo "  ⚠ Failed to checkout main. Creating from develop..."; \
				git checkout -b main; \
			}; \
			git pull origin main 2>/dev/null || true; \
			git merge develop --no-edit -m "Merge develop to main for release [skip ci]" || { \
				echo "  ⚠ Merge conflict detected. Please resolve manually:"; \
				echo "     git checkout main"; \
				echo "     git merge develop"; \
				echo "     # Resolve conflicts, then:"; \
				echo "     git commit"; \
				echo "     git push origin main"; \
				echo "     make github-release SKIP_GIT=1"; \
				exit 1; \
			}; \
			git push origin main || { \
				echo "  ⚠ Failed to push to main. Please push manually:"; \
				echo "     git push origin main"; \
				read -p "  Continue with release anyway? (y/n) " -n 1 -r; \
				echo; \
				if [ "$$REPLY" != "y" ] && [ "$$REPLY" != "Y" ]; then \
					exit 1; \
				fi; \
			}; \
			echo "  ✓ Merged to main and pushed"; \
		elif [ "$$CURRENT_BRANCH" != "main" ]; then \
			echo "Step 3: Switching to main branch..."; \
			git fetch origin main:main 2>/dev/null || true; \
			git checkout main 2>/dev/null || { \
				echo "  ⚠ main branch not found locally or remotely"; \
				exit 1; \
			}; \
			git pull origin main || { \
				echo "  ⚠ Failed to pull latest main"; \
				exit 1; \
			}; \
			echo "  ✓ Switched to main branch"; \
		else \
			echo "Step 3: Already on main branch, pulling latest..."; \
			git pull origin main || { \
				echo "  ⚠ Failed to pull latest main"; \
				exit 1; \
			}; \
			echo "  ✓ Main branch up to date"; \
		fi; \
		echo ""; \
		echo "=== Git operations complete ==="; \
		echo ""; \
	fi; \
	$(MAKE) github-release-build

# Internal target: Build and create GitHub release
# This is called after git operations are complete
github-release-build:
	@echo ""; \
	echo "=== Cleaning release directory ==="; \
	rm -rf $(RELEASE_DIR)/crucible-*; \
	rm -f $(RELEASE_DIR)/notes.md; \
	echo "  ✓ Release directory cleaned"; \
	echo ""; \
	echo "=== Building release binaries ==="; \
	$(MAKE) release; \
	echo ""; \
	echo "=== Creating GitHub release ==="; \
	if ! command -v gh >/dev/null 2>&1; then \
		echo "  ✗ GitHub CLI (gh) not found. Please install it:"; \
		echo "     https://cli.github.com/"; \
		exit 1; \
	fi; \
	if ! gh auth status >/dev/null 2>&1; then \
		echo "  ✗ Not authenticated with GitHub. Please run: gh auth login"; \
		exit 1; \
	fi; \
	echo "  ✓ GitHub CLI found and authenticated"; \
	echo ""; \
	echo "Determining version..."; \
	VERSION=$$( \
		if [ -n "$(VERSION)" ]; then \
			echo "$(VERSION)" | sed 's/^v*/v/'; \
		elif git describe --tags --exact-match >/dev/null 2>&1; then \
			git describe --tags --exact-match; \
		else \
			LATEST_TAG=$$(git describe --tags --abbrev=0 2>/dev/null); \
			if [ -n "$$LATEST_TAG" ]; then \
				MAJOR=$$(echo $$LATEST_TAG | sed 's/^v//' | cut -d. -f1); \
				MINOR=$$(echo $$LATEST_TAG | sed 's/^v//' | cut -d. -f2); \
				PATCH=$$(echo $$LATEST_TAG | sed 's/^v//' | cut -d. -f3); \
				PATCH=$$((PATCH + 1)); \
				echo "v$$MAJOR.$$MINOR.$$PATCH"; \
			else \
				echo "v1.0.0"; \
			fi; \
		fi \
	); \
	if [ -n "$(VERSION)" ]; then \
		echo "  Using specified version: $$VERSION"; \
	elif git describe --tags --exact-match >/dev/null 2>&1; then \
		echo "  Using current tag: $$VERSION"; \
	else \
		LATEST_TAG=$$(git describe --tags --abbrev=0 2>/dev/null); \
		if [ -n "$$LATEST_TAG" ]; then \
			echo "  Auto-incremented from latest tag: $$LATEST_TAG -> $$VERSION"; \
		else \
			echo "  No tags found, using: $$VERSION"; \
		fi; \
	fi; \
	echo ""; \
	echo "Checking if release already exists..."; \
	if gh release view "$$VERSION" >/dev/null 2>&1; then \
		echo "  ⚠ Release $$VERSION already exists on GitHub."; \
		if [ -z "$(VERSION)" ]; then \
			echo "     Auto-incrementing version..."; \
			LATEST_TAG=$$(git describe --tags --abbrev=0 2>/dev/null || echo "v0.0.0"); \
			MAJOR=$$(echo $$LATEST_TAG | sed 's/^v//' | cut -d. -f1); \
			MINOR=$$(echo $$LATEST_TAG | sed 's/^v//' | cut -d. -f2); \
			PATCH=$$(echo $$LATEST_TAG | sed 's/^v//' | cut -d. -f3); \
			PATCH=$$((PATCH + 1)); \
			VERSION="v$$MAJOR.$$MINOR.$$PATCH"; \
			echo "  Using incremented version: $$VERSION"; \
		else \
			echo "     To update it, delete the existing release first, or use a different version."; \
			echo "     Delete: gh release delete $$VERSION"; \
			echo "     Or specify a different version: make github-release VERSION=v1.2.3"; \
			exit 1; \
		fi; \
	fi; \
	echo ""; \
	echo "Creating release notes..."; \
	mkdir -p $(RELEASE_DIR); \
	if [ -f docs/CHANGELOG.md ]; then \
		RELEASE_DATE=$$(date '+%Y-%m-%d %H:%M:%S'); \
		echo "## Release $$VERSION - $$RELEASE_DATE" > $(RELEASE_DIR)/notes.md; \
		echo "" >> $(RELEASE_DIR)/notes.md; \
		awk '/^## \[/{p=0} /^## \[Unreleased\]/{p=1; next} p' docs/CHANGELOG.md > $(RELEASE_DIR)/current-unreleased.md 2>/dev/null; \
		if [ -f .last-changelog.md ]; then \
			echo "  Comparing with previous changelog to extract new changes..."; \
			if [ -s $(RELEASE_DIR)/current-unreleased.md ]; then \
				if ! diff -q .last-changelog.md $(RELEASE_DIR)/current-unreleased.md >/dev/null 2>&1; then \
					diff -u .last-changelog.md $(RELEASE_DIR)/current-unreleased.md 2>/dev/null | \
					awk '/^\+/ && !/^+++/ && !/^\+---/ { \
						line = substr($$0, 2); \
						if (line !~ /^@@/) print line; \
					}' >> $(RELEASE_DIR)/notes.md || \
					cat $(RELEASE_DIR)/current-unreleased.md >> $(RELEASE_DIR)/notes.md; \
				else \
					echo "No new changes detected since last release." >> $(RELEASE_DIR)/notes.md; \
					echo "  ⚠ Warning: [Unreleased] section unchanged since last release"; \
				fi; \
			else \
				echo "No changes found in [Unreleased] section." >> $(RELEASE_DIR)/notes.md; \
			fi; \
		else \
			echo "  No previous changelog found, using all [Unreleased] entries..."; \
			if [ -s $(RELEASE_DIR)/current-unreleased.md ]; then \
				cat $(RELEASE_DIR)/current-unreleased.md >> $(RELEASE_DIR)/notes.md; \
			else \
				echo "No changes found in [Unreleased] section." >> $(RELEASE_DIR)/notes.md; \
			fi; \
		fi; \
		rm -f $(RELEASE_DIR)/current-unreleased.md; \
	else \
		echo "## Changes" > $(RELEASE_DIR)/notes.md; \
		echo "See [CHANGELOG.md](docs/CHANGELOG.md) for details." >> $(RELEASE_DIR)/notes.md; \
	fi; \
	IS_PRERELEASE=$$(echo "$$VERSION" | grep -qE '(-|alpha|beta)' && echo "true" || echo "false"); \
	echo "  Creating release $$VERSION (prerelease: $$IS_PRERELEASE)..."; \
	echo ""; \
	echo "Creating/verifying git tag..."; \
	CURRENT_BRANCH=$$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown"); \
	if [ "$$CURRENT_BRANCH" != "main" ]; then \
		echo "  ⚠ Warning: Not on main branch (currently on $$CURRENT_BRANCH)"; \
		echo "     Tag will be created on current branch"; \
	fi; \
	TAG_EXISTS_LOCAL=$$(git rev-parse "$$VERSION" >/dev/null 2>&1 && echo "yes" || echo "no"); \
	TAG_EXISTS_REMOTE=$$(git ls-remote --tags origin "$$VERSION" >/dev/null 2>&1 && echo "yes" || echo "no"); \
	CURRENT_COMMIT=$$(git rev-parse HEAD); \
	if [ "$$TAG_EXISTS_LOCAL" = "yes" ]; then \
		TAG_COMMIT=$$(git rev-parse "$$VERSION" 2>/dev/null); \
		if [ "$$TAG_COMMIT" != "$$CURRENT_COMMIT" ]; then \
			echo "  ⚠ Warning: Tag $$VERSION exists but points to different commit"; \
			echo "     Tag points to: $$TAG_COMMIT"; \
			echo "     Current HEAD:  $$CURRENT_COMMIT"; \
			echo "     Deleting local tag to recreate..."; \
			git tag -d "$$VERSION" 2>/dev/null || true; \
			TAG_EXISTS_LOCAL="no"; \
		fi; \
	fi; \
	if [ "$$TAG_EXISTS_LOCAL" = "no" ]; then \
		echo "  Creating git tag $$VERSION..."; \
		git tag -a "$$VERSION" -m "Release $$VERSION" || { \
			echo "  ✗ Failed to create tag"; \
			exit 1; \
		}; \
		echo "  ✓ Tag created locally"; \
	fi; \
	if [ "$$TAG_EXISTS_REMOTE" = "no" ]; then \
		echo "  Pushing tag to remote..."; \
		git push origin "$$VERSION" || { \
			echo "  ✗ Failed to push tag to remote"; \
			echo "     Tag exists locally but not on remote."; \
			echo "     Please push manually: git push origin $$VERSION"; \
			echo "     Or delete local tag and retry: git tag -d $$VERSION"; \
			exit 1; \
		}; \
		echo "  ✓ Tag pushed to remote"; \
	elif [ "$$TAG_EXISTS_LOCAL" = "yes" ]; then \
		echo "  ✓ Tag $$VERSION already exists on remote"; \
	fi; \
	echo ""; \
	echo "Renaming release files to include version..."; \
	VERSION_NUM=$$(echo "$$VERSION" | sed 's/^v//'); \
	if [ -f "$(RELEASE_DIR)/crucible-linux-amd64" ]; then \
		mv "$(RELEASE_DIR)/crucible-linux-amd64" "$(RELEASE_DIR)/crucible-$$VERSION_NUM-linux-amd64"; \
		echo "  ✓ crucible-$$VERSION_NUM-linux-amd64"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-linux-arm64" ]; then \
		mv "$(RELEASE_DIR)/crucible-linux-arm64" "$(RELEASE_DIR)/crucible-$$VERSION_NUM-linux-arm64"; \
		echo "  ✓ crucible-$$VERSION_NUM-linux-arm64"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-windows-x86_64.exe" ]; then \
		mv "$(RELEASE_DIR)/crucible-windows-x86_64.exe" "$(RELEASE_DIR)/crucible-$$VERSION_NUM-windows-x86_64.exe"; \
		echo "  ✓ crucible-$$VERSION_NUM-windows-x86_64.exe"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-macos-x86_64" ]; then \
		mv "$(RELEASE_DIR)/crucible-macos-x86_64" "$(RELEASE_DIR)/crucible-$$VERSION_NUM-macos-x86_64"; \
		echo "  ✓ crucible-$$VERSION_NUM-macos-x86_64"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-macos-arm64" ]; then \
		mv "$(RELEASE_DIR)/crucible-macos-arm64" "$(RELEASE_DIR)/crucible-$$VERSION_NUM-macos-arm64"; \
		echo "  ✓ crucible-$$VERSION_NUM-macos-arm64"; \
	fi; \
	echo ""; \
	RELEASE_FILES=""; \
	if [ -f "$(RELEASE_DIR)/crucible-$$VERSION_NUM-linux-amd64" ]; then \
		RELEASE_FILES="$(RELEASE_DIR)/crucible-$$VERSION_NUM-linux-amd64"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-$$VERSION_NUM-linux-arm64" ]; then \
		RELEASE_FILES="$$RELEASE_FILES $(RELEASE_DIR)/crucible-$$VERSION_NUM-linux-arm64"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-$$VERSION_NUM-windows-x86_64.exe" ]; then \
		RELEASE_FILES="$$RELEASE_FILES $(RELEASE_DIR)/crucible-$$VERSION_NUM-windows-x86_64.exe"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-$$VERSION_NUM-macos-x86_64" ]; then \
		RELEASE_FILES="$$RELEASE_FILES $(RELEASE_DIR)/crucible-$$VERSION_NUM-macos-x86_64"; \
	fi; \
	if [ -f "$(RELEASE_DIR)/crucible-$$VERSION_NUM-macos-arm64" ]; then \
		RELEASE_FILES="$$RELEASE_FILES $(RELEASE_DIR)/crucible-$$VERSION_NUM-macos-arm64"; \
	fi; \
	if [ -z "$$RELEASE_FILES" ]; then \
		echo "  ✗ No release files found to upload"; \
		exit 1; \
	fi; \
	echo "Release files to upload:"; \
	for file in $$RELEASE_FILES; do \
		if [ -f "$$file" ]; then \
			echo "  ✓ $$(basename $$file)"; \
		else \
			echo "  ✗ Missing: $$file"; \
		fi; \
	done; \
	echo ""; \
	echo "Verifying tag exists on remote..."; \
	if ! git ls-remote --tags origin "$$VERSION" >/dev/null 2>&1; then \
		echo "  ✗ Tag $$VERSION does not exist on remote"; \
		echo "     Please push the tag first: git push origin $$VERSION"; \
		exit 1; \
	fi; \
	echo "  ✓ Tag $$VERSION exists on remote"; \
	echo ""; \
	echo "Uploading release files..."; \
	gh release create "$$VERSION" \
		--title "Release $$VERSION" \
		--notes-file $(RELEASE_DIR)/notes.md \
		$$RELEASE_FILES \
		--prerelease=$$IS_PRERELEASE \
		--latest || { \
		echo ""; \
		echo "  ✗ Failed to create release."; \
		echo "     Common issues:"; \
		echo "     - Release with tag $$VERSION already exists"; \
		echo "     - Tag $$VERSION doesn't exist (should have been created above)"; \
		echo "     - Insufficient permissions"; \
		echo ""; \
		echo "     Solutions:"; \
		echo "     - Delete existing release: gh release delete $$VERSION"; \
		echo "     - Use a different version: make github-release VERSION=v1.2.3"; \
		echo "     - Check permissions: gh auth status"; \
		exit 1; \
	}; \
	echo ""; \
	echo "  ✓ GitHub release created successfully!"; \
	REPO=$$(gh repo view --json owner,name -q '.owner.login + "/" + .name' 2>/dev/null); \
	if [ -n "$$REPO" ]; then \
		echo "  View it at: https://github.com/$$REPO/releases/latest"; \
	fi; \
	echo ""; \
	echo "Saving changelog state..."; \
	if [ -f docs/CHANGELOG.md ]; then \
		awk '/^## \[/{p=0} /^## \[Unreleased\]/{p=1; next} p' docs/CHANGELOG.md > .last-changelog.md 2>/dev/null && \
		echo "  ✓ Saved current [Unreleased] section to .last-changelog.md" || \
		echo "  ⚠ Failed to save changelog state"; \
	else \
		echo "  ⚠ CHANGELOG.md not found, skipping state save"; \
	fi

# Linux amd64 build (native)
release-linux-amd64:
	@mkdir -p $(RELEASE_DIR)
	@echo "Building Linux amd64..."
	@rm -rf $(BDIR)-linux-amd64
	@$(MAKE) CC=gcc CFLAGS="$(INCLUDES) -O3 -Wall -flto=auto" BDIR=$(BDIR)-linux-amd64 TARGET=$(BDIR)-linux-amd64/crucible
	@cp $(BDIR)-linux-amd64/crucible $(RELEASE_DIR)/crucible-linux-amd64
	@chmod +x $(RELEASE_DIR)/crucible-linux-amd64
	@echo "  ✓ Linux amd64 build complete"

# Linux arm64 build (cross-compile)
release-linux-arm64:
	@mkdir -p $(RELEASE_DIR)
	@echo "Building Linux arm64..."
	@if ! command -v aarch64-linux-gnu-gcc >/dev/null 2>&1; then \
		echo "  ⚠ aarch64-linux-gnu-gcc not found. Installing cross-compiler..."; \
		if command -v apt-get >/dev/null 2>&1; then \
			sudo apt-get update && sudo apt-get install -y gcc-aarch64-linux-gnu; \
		elif command -v yum >/dev/null 2>&1; then \
			sudo yum install -y gcc-aarch64-linux-gnu; \
		else \
			echo "  ✗ Please install gcc-aarch64-linux-gnu manually"; \
			exit 1; \
		fi \
	fi
	@rm -rf $(BDIR)-linux-arm64
	@$(MAKE) CC=aarch64-linux-gnu-gcc CFLAGS="$(INCLUDES) -O3 -Wall -flto=auto" BDIR=$(BDIR)-linux-arm64 TARGET=$(BDIR)-linux-arm64/crucible
	@cp $(BDIR)-linux-arm64/crucible $(RELEASE_DIR)/crucible-linux-arm64
	@chmod +x $(RELEASE_DIR)/crucible-linux-arm64
	@echo "  ✓ Linux arm64 build complete"

# Windows x86_64 build (cross-compile with MinGW)
release-windows-x86_64:
	@mkdir -p $(RELEASE_DIR)
	@echo "Building Windows x86_64..."
	@if ! command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then \
		echo "  ⚠ x86_64-w64-mingw32-gcc not found. Installing MinGW..."; \
		if command -v apt-get >/dev/null 2>&1; then \
			sudo apt-get update && sudo apt-get install -y mingw-w64; \
		elif command -v yum >/dev/null 2>&1; then \
			sudo yum install -y mingw64-gcc; \
		else \
			echo "  ✗ Please install mingw-w64 manually"; \
			exit 1; \
		fi \
	fi
	@rm -rf $(BDIR)-windows-x86_64
	@$(MAKE) CC=x86_64-w64-mingw32-gcc CFLAGS="$(INCLUDES) -O3 -Wall -flto=auto" BDIR=$(BDIR)-windows-x86_64 TARGET=$(BDIR)-windows-x86_64/crucible.exe
	@cp $(BDIR)-windows-x86_64/crucible.exe $(RELEASE_DIR)/crucible-windows-x86_64.exe
	@echo "  ✓ Windows x86_64 build complete"

$(ODIR)/%.o: src/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BDIR) $(ODIR):
	mkdir -p $@


$(ODIR)/atari.o: src/atari.c src/atari.h src/sim65.h src/atcio.h src/atsio.h \
	src/mathpack.h src/hw.h
$(ODIR)/atcio.o: src/atcio.c src/atcio.h src/sim65.h src/atari.h src/dosfname.h src/atrdev.h src/athost.h
$(ODIR)/atrdev.o: src/atrdev.c src/atrdev.h src/sim65.h src/atcio.h src/atari.h src/hw.h
$(ODIR)/atrfs.o: src/atrfs.c src/atrfs.h src/atsio.h src/atari.h
$(ODIR)/athost.o: src/athost.c src/athost.h src/atcio.h src/atari.h src/ciodev.h src/dosfname.h
$(ODIR)/atsio.o: src/atsio.c src/atsio.h src/sim65.h src/atari.h
$(ODIR)/dosfname.o: src/dosfname.c src/dosfname.h
$(ODIR)/ataridos.o: src/ataridos.c src/ataridos.h src/atari.h src/atcio.h src/ciodev.h src/dosfname.h src/atrfs.h src/atsio.h
$(ODIR)/hw.o: src/hw.c src/hw.h src/sim65.h
$(ODIR)/main.o: src/main.c src/atari.h src/sim65.h
$(ODIR)/mathpack.o: src/mathpack.c src/mathpack.h src/sim65.h src/mathpack_bin.h
$(ODIR)/sim65.o: src/sim65.c src/sim65.h

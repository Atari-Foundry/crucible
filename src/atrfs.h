/*
 * crucible - Small 6502 simulator with Atari 8bit bios.
 * Copyright (C) 2026 AtariFoundry.com
 * Copyright (C) 2017-2019 Daniel Serpell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include "atsio.h"
#include "sim65.h"
#include <stdint.h>

// File handle for ATR-based files
struct atr_file_handle
{
    uint16_t start_sector;
    uint16_t current_sector;
    uint32_t file_size;
    uint32_t position;
    uint8_t sector_buffer[256];
    int sector_pos;
    int sector_size;
    sim65 sim; // Reference to simulator for sector access
};

// Directory entry
struct atr_dir_entry
{
    uint8_t flags;
    uint16_t start_sector;
    uint16_t sector_count;
    uint32_t byte_count;
    char filename[12];
    // Extended attributes (for SpartaDOS/BW-DOS)
    uint8_t attributes; // Additional file attributes
    int is_hidden;      // Hidden file flag
    int is_protected;   // Protected file flag
    int is_archived;    // Archived file flag
};

// Functions
// Find a file in the ATR filesystem by path (e.g., "FILE.EXT" or "DIR\FILE.EXT")
// Returns 0 on success, -1 on error
int atrfs_find_file(sim65 s, const char *path, struct atr_dir_entry *entry);

// Open a file handle for reading
// Returns NULL on error
struct atr_file_handle *atrfs_open_file(sim65 s, uint16_t start_sector, uint32_t size);

// Read a byte from file, returns byte value or -1 on EOF/error
int atrfs_read_byte(struct atr_file_handle *fh);

// Seek to position in file (returns 0 on success, -1 on error)
int atrfs_seek(struct atr_file_handle *fh, uint32_t position);

// Close and free file handle
void atrfs_close_file(struct atr_file_handle *fh);

// Read directory entries from a directory sector
// Returns number of entries found, or -1 on error
int atrfs_read_directory(sim65 s, uint16_t dir_sector, struct atr_dir_entry *entries,
                         int max_entries);

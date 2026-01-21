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

/* Implements ATR filesystem access */
#include "atrfs.h"
#include "atsio.h"
#include "atari.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// DOS 2.x directory entry flags
#define DIR_FLAG_DELETED   0x80
#define DIR_FLAG_INUSE     0x40
#define DIR_FLAG_LOCKED    0x20
#define DIR_FLAG_SUBDIR    0x10 // MyDOS subdirectory
#define DIR_FLAG_EXTFILE   0x04 // MyDOS extended file
#define DIR_FLAG_DOS2      0x02
#define DIR_FLAG_OPENWRITE 0x01

// VTOC location (sector 360)
#define VTOC_SECTOR 360
// Root directory typically starts at sector 361
#define ROOT_DIR_SECTOR 361
#define DIR_SECTORS 8 // 8 sectors for directory (64 entries)

// Helper: Read a sector from ATR image
static int read_sector(sim65 s, unsigned sector, uint8_t *buffer)
{
    unsigned size;
    if (atari_sio_read_sector(s, sector, buffer, &size) != 0)
        return -1;
    return (int)size;
}

// Read VTOC to get directory location
static int read_vtoc(sim65 s, uint16_t *dir_sector)
{
    uint8_t vtoc[128];
    int size = read_sector(s, VTOC_SECTOR, vtoc);
    if (size < 0)
        return -1;

    // VTOC format: byte 3-4 contains directory start sector
    // For DOS 2.x, this is typically 361
    *dir_sector = vtoc[3] | (vtoc[4] << 8);
    if (*dir_sector == 0)
        *dir_sector = ROOT_DIR_SECTOR; // Default if not set

    return 0;
}

// Helper: Get next sector from sector link (last 3 bytes of sector)
static uint16_t get_next_sector(const uint8_t *sector_data, int sector_size, int is_dos1)
{
    // For DOS 2.x: last 3 bytes contain next sector (little-endian, 24-bit)
    // For DOS 1.x: similar but different format
    if (sector_size == 128)
    {
        // 128-byte sector: bytes 125-127 contain next sector
        uint16_t next = sector_data[125] | (sector_data[126] << 8);
        // Check if extended addressing (byte 127 has high byte)
        if (sector_data[127] != 0)
            next |= (sector_data[127] << 16);
        return next;
    }
    else
    {
        // 256-byte sector: bytes 253-255 contain next sector
        uint16_t next = sector_data[253] | (sector_data[254] << 8);
        if (sector_data[255] != 0)
            next |= (sector_data[255] << 16);
        return next;
    }
}

// Helper: Get data bytes in a sector (excluding link bytes)
static int get_sector_data_bytes(int sector_size, int is_dos1)
{
    (void)is_dos1; // For now, assume DOS 2.x
    if (sector_size == 128)
        return 125; // 128 - 3 link bytes
    else
        return 253; // 256 - 3 link bytes
}

// Parse a directory entry (16 bytes)
static void parse_dir_entry(const uint8_t *data, struct atr_dir_entry *entry)
{
    entry->flags        = data[0];
    entry->start_sector = data[1] | (data[2] << 8);
    entry->sector_count = data[3] | (data[4] << 8);
    entry->byte_count  = data[5] | (data[6] << 8) | (data[7] << 16);
    // Filename is 11 bytes (8.3 format)
    memcpy(entry->filename, data + 8, 11);
    entry->filename[11] = '\0';
    // Trim trailing spaces
    for (int i = 10; i >= 0; i--)
    {
        if (entry->filename[i] == ' ')
            entry->filename[i] = '\0';
        else
            break;
    }

    // Parse extended attributes
    entry->attributes   = 0;
    entry->is_hidden    = 0;
    entry->is_protected = 0;
    entry->is_archived  = 0;

    // Check for locked files (bit 5 of flags)
    if (entry->flags & DIR_FLAG_LOCKED)
        entry->is_protected = 1;

    // For SpartaDOS/BW-DOS: additional attribute parsing
    // Some DOS versions use filename patterns or additional bytes
    // For now, we rely on the standard flags byte
}

// Find subdirectory by name and return its start sector
static int find_subdir(sim65 s, uint16_t dir_sector, const char *dirname,
                       uint16_t *subdir_sector)
{
    const struct atr_disk_image *img = atari_sio_get_disk_image(s);
    if (!img)
        return -1;

    uint8_t sector_buffer[256];
    (void)img; // sector_size not used yet

    for (int dir_sec = 0; dir_sec < DIR_SECTORS; dir_sec++)
    {
        unsigned sector_num = dir_sector + dir_sec;
        int size           = read_sector(s, sector_num, sector_buffer);
        if (size < 0)
            continue;

        for (int i = 0; i < 8; i++)
        {
            const uint8_t *dirent = sector_buffer + (i * 16);
            uint8_t flags         = dirent[0];

            if (flags == 0)
                return -1;

            if (flags & DIR_FLAG_DELETED)
                continue;

            // Check if this is a subdirectory
            if (flags & DIR_FLAG_SUBDIR)
            {
                struct atr_dir_entry entry;
                parse_dir_entry(dirent, &entry);

                // Compare directory name
                char entry_name[13];
                int j;
                for (j = 0; j < 8 && entry.filename[j] && entry.filename[j] != ' '; j++)
                    entry_name[j] = entry.filename[j] | 0x20;
                entry_name[j] = '\0';

                int match = 1;
                for (int k = 0; entry_name[k] || dirname[k]; k++)
                {
                    if (tolower((unsigned char)entry_name[k]) !=
                        tolower((unsigned char)dirname[k]))
                    {
                        match = 0;
                        break;
                    }
                }

                if (match)
                {
                    *subdir_sector = entry.start_sector;
                    return 0;
                }
            }
        }
    }

    return -1;
}

// Find file in directory (recursive for subdirectories)
static int find_file_in_dir(sim65 s, uint16_t dir_sector, const char *filename,
                            struct atr_dir_entry *entry)
{
    const struct atr_disk_image *img = atari_sio_get_disk_image(s);
    if (!img)
        return -1;

    uint8_t sector_buffer[256];

    // Search through directory sectors (typically 8 sectors)
    for (int dir_sec = 0; dir_sec < DIR_SECTORS; dir_sec++)
    {
        unsigned sector_num = dir_sector + dir_sec;
        int size           = read_sector(s, sector_num, sector_buffer);
        if (size < 0)
            continue;

        // Each sector has 8 directory entries (16 bytes each)
        for (int i = 0; i < 8; i++)
        {
            const uint8_t *dirent = sector_buffer + (i * 16);
            uint8_t flags         = dirent[0];

            // End of directory
            if (flags == 0)
                return -1;

            // Skip deleted entries
            if (flags & DIR_FLAG_DELETED)
                continue;

            parse_dir_entry(dirent, entry);

            // Check if this is a subdirectory
            if (flags & DIR_FLAG_SUBDIR)
            {
                // Skip subdirectories in file search (they're handled separately)
                continue;
            }

            // Skip hidden files if requested (for future enhancement)
            // For now, we include all visible files

            // Compare filename (case-insensitive, handle 8.3 format)
            char entry_name[13];
            int j;
            for (j = 0; j < 8 && entry->filename[j] && entry->filename[j] != ' '; j++)
                entry_name[j] = entry->filename[j] | 0x20; // lowercase
            entry_name[j] = '\0';

            // Check for extension
            if (entry->filename[8] != ' ')
            {
                entry_name[j++] = '.';
                for (int k = 8; k < 11 && entry->filename[k] != ' '; k++)
                    entry_name[j++] = entry->filename[k] | 0x20;
                entry_name[j] = '\0';
            }

            // Simple filename comparison (case-insensitive)
            // TODO: Handle wildcards, path parsing
            int match = 1;
            for (int k = 0; entry_name[k] || filename[k]; k++)
            {
                if (tolower((unsigned char)entry_name[k]) != tolower((unsigned char)filename[k]))
                {
                    match = 0;
                    break;
                }
            }
            if (match)
            {
                return 0; // Found!
            }
        }
    }

    return -1; // Not found
}

int atrfs_find_file(sim65 s, const char *path, struct atr_dir_entry *entry)
{
    if (!s || !path || !entry)
        return -1;

    const struct atr_disk_image *img = atari_sio_get_disk_image(s);
    if (!img)
        return -1;

    // Read VTOC to get directory location
    uint16_t dir_sector = ROOT_DIR_SECTOR;
    read_vtoc(s, &dir_sector);

    // Parse path: handle "DIR\FILE.EXT" or just "FILE.EXT"
    const char *backslash = strchr(path, '\\');
    const char *filename  = path;

    if (backslash)
    {
        // Extract directory name
        int dirname_len = backslash - path;
        char dirname[9];
        if (dirname_len > 8)
            dirname_len = 8;
        memcpy(dirname, path, dirname_len);
        dirname[dirname_len] = '\0';

        // Find subdirectory
        uint16_t subdir_sector;
        if (find_subdir(s, dir_sector, dirname, &subdir_sector) != 0)
            return -1;

        dir_sector = subdir_sector;
        filename   = backslash + 1;
    }

    // Search in the appropriate directory
    return find_file_in_dir(s, dir_sector, filename, entry);
}

struct atr_file_handle *atrfs_open_file(sim65 s, uint16_t start_sector, uint32_t size)
{
    if (!s || start_sector == 0)
        return NULL;

    struct atr_file_handle *fh = calloc(1, sizeof(struct atr_file_handle));
    if (!fh)
        return NULL;

    const struct atr_disk_image *img = atari_sio_get_disk_image(s);
    if (!img)
    {
        free(fh);
        return NULL;
    }

    fh->sim           = s;
    fh->start_sector  = start_sector;
    fh->current_sector = start_sector;
    fh->file_size     = size;
    fh->position      = 0;
    fh->sector_size   = (int)img->sec_size;
    fh->sector_pos    = 0;

    // Load first sector
    unsigned sector_size;
    if (atari_sio_read_sector(s, start_sector, fh->sector_buffer, &sector_size) != 0)
    {
        free(fh);
        return NULL;
    }

    return fh;
}

int atrfs_read_byte(struct atr_file_handle *fh)
{
    if (!fh || fh->position >= fh->file_size)
        return -1;

    const struct atr_disk_image *img = atari_sio_get_disk_image(fh->sim);
    if (!img)
        return -1;

    int sector_data_bytes = get_sector_data_bytes(fh->sector_size, 0);

    // Check if we need to load next sector
    if (fh->sector_pos >= sector_data_bytes)
    {
        // Get next sector from link
        uint16_t next_sector = get_next_sector(fh->sector_buffer, fh->sector_size, 0);
        if (next_sector == 0 || next_sector > img->sec_count)
            return -1; // End of file or invalid sector

        fh->current_sector = next_sector;
        fh->sector_pos    = 0;

        // Load next sector
        unsigned sector_size;
        if (atari_sio_read_sector(fh->sim, next_sector, fh->sector_buffer, &sector_size) != 0)
            return -1;
    }

    // Read byte from current position
    int byte = fh->sector_buffer[fh->sector_pos++];
    fh->position++;
    return byte;
}

int atrfs_seek(struct atr_file_handle *fh, uint32_t position)
{
    if (!fh || position > fh->file_size)
        return -1;

    // For now, simple implementation: reset and read to position
    // TODO: Optimize to jump directly to sector containing position
    fh->position      = 0;
    fh->current_sector = fh->start_sector;
    fh->sector_pos    = 0;

    // Load first sector
    unsigned sector_size;
    if (atari_sio_read_sector(fh->sim, fh->start_sector, fh->sector_buffer, &sector_size) != 0)
        return -1;

    // Read bytes until we reach position
    int sector_data_bytes = get_sector_data_bytes(fh->sector_size, 0);
    const struct atr_disk_image *img = atari_sio_get_disk_image(fh->sim);
    if (!img)
        return -1;

    while (fh->position < position)
    {
        if (fh->sector_pos >= sector_data_bytes)
        {
            // Move to next sector
            uint16_t next_sector = get_next_sector(fh->sector_buffer, fh->sector_size, 0);
            if (next_sector == 0 || next_sector > img->sec_count)
                return -1;

            fh->current_sector = next_sector;
            fh->sector_pos    = 0;

            if (atari_sio_read_sector(fh->sim, next_sector, fh->sector_buffer, &sector_size) !=
                0)
                return -1;
        }

        fh->sector_pos++;
        fh->position++;
    }

    return 0;
}

void atrfs_close_file(struct atr_file_handle *fh)
{
    if (fh)
        free(fh);
}

int atrfs_read_directory(sim65 s, uint16_t dir_sector, struct atr_dir_entry *entries,
                         int max_entries)
{
    if (!s || !entries || max_entries <= 0)
        return -1;

    const struct atr_disk_image *img = atari_sio_get_disk_image(s);
    if (!img)
        return -1;

    // If dir_sector is 0, read from VTOC
    if (dir_sector == 0)
    {
        if (read_vtoc(s, &dir_sector) != 0)
            return -1;
    }

    uint8_t sector_buffer[256];
    int count = 0;

    // Read directory sectors
    for (int dir_sec = 0; dir_sec < DIR_SECTORS && count < max_entries; dir_sec++)
    {
        unsigned sector_num = dir_sector + dir_sec;
        int size           = read_sector(s, sector_num, sector_buffer);
        if (size < 0)
            continue;

        // Parse entries in this sector
        for (int i = 0; i < 8 && count < max_entries; i++)
        {
            const uint8_t *dirent = sector_buffer + (i * 16);
            uint8_t flags        = dirent[0];

            // End of directory
            if (flags == 0)
                return count;

            // Skip deleted entries
            if (flags & DIR_FLAG_DELETED)
                continue;

            // Parse entry (includes extended attributes)
            parse_dir_entry(dirent, &entries[count]);
            
            // Include all visible entries (files and subdirectories)
            // Hidden files are still included for now (can be filtered later if needed)
            count++;
        }
    }

    return count;
}

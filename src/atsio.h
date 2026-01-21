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

#include "sim65.h"
#include <stdint.h>

// Init SIO emulation
void atari_sio_init(sim65 s);
// Boot from loaded disk image
enum sim65_error atari_sio_boot(sim65 s);
// Load a disk image
int atari_sio_load_image(sim65 s, const char *file_name);

// ATR filesystem access API
struct atr_disk_image
{
    uint8_t *data;
    unsigned sec_size;
    unsigned sec_count;
};

// Get pointer to disk image structure (returns NULL if no image loaded)
const struct atr_disk_image *atari_sio_get_disk_image(sim65 s);
// Read a sector from the ATR image (sector is 1-based, returns 0 on success)
int atari_sio_read_sector(sim65 s, unsigned sector, uint8_t *buffer, unsigned *size);

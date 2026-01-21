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

/* Implements H: Device (Host filesystem) handler */
#include "athost.h"
#include "atcio.h"
#include "atari.h"
#include "ciodev.h"
#include "dosfname.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static const char *host_root_path = ".";

// Misc routines
static unsigned peek(sim65 s, unsigned addr)
{
    return sim65_get_byte(s, addr);
}

static unsigned dpeek(sim65 s, unsigned addr)
{
    return sim65_get_byte(s, addr) + (sim65_get_byte(s, addr + 1) << 8);
}

static void poke(sim65 s, unsigned addr, unsigned char val)
{
    sim65_add_data_ram(s, addr, &val, 1);
}

// H: device handler table
static const unsigned char devhand_host[] = {
    DEVH_TAB(HOST)
};

static int sim_HOST(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    // Store one file handle for each CIO channel
    static FILE *fhand[16];
    static DIR *dir_hand[16] = {0}; // Directory handles for mode 6

    // We need IOCB data
    unsigned chn  = (regs->x >> 4);
    unsigned cmd  = peek(s, ICCOMZ);
    unsigned badr = dpeek(s, ICBALZ);
    unsigned dno  = peek(s, ICDNOZ);
    unsigned ax1  = peek(s, ICAX1Z);
    unsigned ax2  = peek(s, ICAX2Z);

    switch (addr & 7)
    {
        case DEVR_OPEN:
        {
            // Decode file name
            char fname[256];
            int i;
            // Skip 'H#:'
            badr++;
            if (dno == (peek(s, badr) - '0'))
                badr++;
            if (peek(s, badr) == ':')
                badr++;
            // Translate rest of filename
            for (i = 0; i < 250; i++)
            {
                int c = peek(s, badr++);
                if (!c || c == 0x9b)
                    break;
                fname[i] = c;
            }
            fname[i] = 0;
            sim65_dprintf(s, "HOST OPEN #%d, %d, %d, '%s'", chn, ax1, ax2, fname);

            // Test if not already open
            if (fhand[chn])
            {
                sim65_dprintf(s, "HOST: Internal error, %d already open.", chn);
                fclose(fhand[chn]);
                fhand[chn] = 0;
            }
            if (dir_hand[chn])
            {
                closedir(dir_hand[chn]);
                dir_hand[chn] = 0;
            }

            // Open Flag:
            const char *flags = 0;
            switch (ax1)
            {
                case 4: // Open for read
                    flags = "rb";
                    break;
                case 8: // Open for write
                    flags = "wb";
                    break;
                case 9: // Open for append
                    flags = "ab";
                    break;
                case 12: // Open for update
                    flags = "r+b";
                    break;
                case 6: // Directory read
                {
                    // Open directory for reading
                    char fullpath[512];
                    if (fname[0] == '\0')
                    {
                        // No path specified, use root
                        snprintf(fullpath, sizeof(fullpath), "%s", host_root_path);
                    }
                    else
                    {
                        snprintf(fullpath, sizeof(fullpath), "%s/%s", host_root_path, fname);
                    }

                    dir_hand[chn] = opendir(fullpath);
                    if (!dir_hand[chn])
                    {
                        sim65_dprintf(s, "HOST DIR OPEN: error %s", strerror(errno));
                        if (errno == ENOENT)
                            regs->y = 170;
                        else if (errno == ENOSPC)
                            regs->y = 162;
                        else if (errno == EACCES)
                            regs->y = 167;
                        else
                            regs->y = 139;
                    }
                    else
                    {
                        regs->y = 1;
                    }
                    return 0;
                }
                default:
                    regs->y = 0xA8;
                    return 0;
            }

            fhand[chn] = dosfopen(host_root_path, fname, flags);
            if (!fhand[chn])
            {
                sim65_dprintf(s, "HOST OPEN: error %s", strerror(errno));
                if (errno == ENOENT)
                    regs->y = 170;
                else if (errno == ENOSPC)
                    regs->y = 162;
                else if (errno == EACCES)
                    regs->y = 167;
                else
                    regs->y = 139;
            }
            else
                regs->y = 1;
            return 0;
        }
        case DEVR_CLOSE:
            if (fhand[chn])
            {
                fclose(fhand[chn]);
                fhand[chn] = 0;
            }
            if (dir_hand[chn])
            {
                closedir(dir_hand[chn]);
                dir_hand[chn] = 0;
            }
            regs->y = 1;
            return 0;
        case DEVR_GET:
            if (dir_hand[chn])
            {
                // Directory read mode
                struct dirent *entry = readdir(dir_hand[chn]);
                if (!entry)
                {
                    regs->y = 136; // EOF
                }
                else
                {
                    // Format directory entry (simplified)
                    // Return filename character by character
                    static char dir_buffer[256];
                    static int dir_pos = 0;
                    static int dir_len = 0;

                    if (dir_pos >= dir_len)
                    {
                        // Format new entry
                        struct stat st;
                        char fullpath[512];
                        snprintf(fullpath, sizeof(fullpath), "%s/%s", host_root_path,
                                 entry->d_name);
                        stat(fullpath, &st);

                        // Format: "FILENAME  EXT 12345" (DOS 2.x style)
                        dir_len = snprintf(dir_buffer, sizeof(dir_buffer), "%-11s %5lu",
                                           entry->d_name, (unsigned long)st.st_size);
                        dir_pos = 0;
                    }

                    if (dir_pos < dir_len)
                    {
                        regs->a = dir_buffer[dir_pos++];
                        regs->y = 1;
                    }
                    else
                    {
                        regs->a = 0x9B; // EOL
                        regs->y = 1;
                        dir_pos = dir_len = 0; // Reset for next entry
                    }
                }
            }
            else if (!fhand[chn])
            {
                sim65_dprintf(s, "HOST GET: Internal error, %d closed.", chn);
                regs->y = 133;
            }
            else
            {
                int c   = fgetc(fhand[chn]);
                regs->y = 1;
                if (c == EOF)
                    regs->y = 136;
                else
                    regs->a = c;
            }
            return 0;
        case DEVR_PUT:
            if (!fhand[chn])
            {
                sim65_dprintf(s, "HOST PUT: Internal error, %d closed.", chn);
                regs->y = 133;
            }
            else
            {
                fputc(regs->a, fhand[chn]);
                regs->y = 1;
            }
            return 0;
        case DEVR_STATUS:
            return 0;
        case DEVR_SPECIAL:
            if (cmd == 37) // POINT
            {
                unsigned ax3 = peek(s, regs->x + ICAX3);
                unsigned ax4 = peek(s, regs->x + ICAX4);
                unsigned ax5 = peek(s, regs->x + ICAX5);
                if (!fhand[chn])
                {
                    sim65_dprintf(s, "HOST POINT: %d closed.", chn);
                    regs->y = 133;
                }
                else
                {
                    sim65_dprintf(s, "HOST POINT: $%02x.%02x.%02x", ax5, ax4, ax3);
                    long offset = (ax5 << 16) | (ax4 << 8) | ax3;
                    fseek(fhand[chn], offset, SEEK_SET);
                    regs->y = 1;
                }
            }
            else if (cmd == 38) // NOTE
            {
                if (!fhand[chn])
                {
                    sim65_dprintf(s, "HOST NOTE: %d closed.", chn);
                    regs->y = 133;
                }
                else
                {
                    long offset  = ftell(fhand[chn]);
                    regs->y      = 1;
                    unsigned ax3 = offset & 0xFF;
                    unsigned ax4 = (offset >> 8) & 0xFF;
                    unsigned ax5 = (offset >> 16) & 0xFF;
                    poke(s, regs->x + ICAX3, ax3);
                    poke(s, regs->x + ICAX4, ax4);
                    poke(s, regs->x + ICAX5, ax5);
                    sim65_dprintf(s, "HOST NOTE = $%02x.%02x.%02x", ax5, ax4, ax3);
                }
            }
            return 0;
        case DEVR_INIT:
            return 0;
        default:
            sim65_eprintf(s, "invalid access to cb address $%04x", addr);
            return 0;
    }
}

void atari_host_set_root(sim65 s, const char *path)
{
    (void)s; // Unused parameter
    if (path)
        host_root_path = path;
    else
        host_root_path = ".";
}

void atari_host_init(sim65 s)
{
    sim65_add_data_rom(s, HOSTV, devhand_host, sizeof(devhand_host));
    // Register handler callback
    add_rts_callback(s, HOST_BASE, 8, sim_HOST);
    // Add device to handler table
    atari_cio_add_hatab(s, 'H', HOSTV);
}

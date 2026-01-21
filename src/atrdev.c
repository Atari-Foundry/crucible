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

/* Implements R: Device (RS-232/Serial) handler */
#include "atrdev.h"
#include "atcio.h"
#include "atari.h"
#include "ciodev.h"
#include "hw.h"
#include "sim65.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Utility functions
static unsigned peek(sim65 s, unsigned addr)
{
    return sim65_get_byte(s, addr);
}


// R: device state per channel (up to 8 channels)
#define MAX_RDEV_CHANNELS 8
struct rdev_channel_state
{
    int is_open;
    int baud_rate;      // Baud rate index
    int translation;    // Translation mode
    FILE *input_file;   // Input file (NULL = stdin)
    FILE *output_file;  // Output file (NULL = stdout)
};

static struct rdev_channel_state rdev_channels[MAX_RDEV_CHANNELS] = {0};

// Baud rate table (common rates)
static const int baud_rates[] = {
    300,  600,  1200, 2400, 4800, 9600, 19200, 38400,
    57600, 115200, 0, 0, 0, 0, 0, 0
};

// Get baud rate from AUX1 value
static int get_baud_rate(int aux1)
{
    int index = aux1 & 0x0F;
    if (index < sizeof(baud_rates) / sizeof(baud_rates[0]) && baud_rates[index] > 0)
        return baud_rates[index];
    return 1200; // Default baud rate
}

static int cb_error(sim65 s, unsigned addr)
{
    sim65_eprintf(s, "invalid access to cb address $%04x", addr);
    return 0;
}

static int sim_RDEV(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    unsigned channel = peek(s, ICIDNO) >> 4;
    if (channel >= MAX_RDEV_CHANNELS)
    {
        regs->y = 0x82; // Unknown device
        return 0;
    }

    struct rdev_channel_state *ch = &rdev_channels[channel];

    switch (addr & 7)
    {
        case DEVR_OPEN:
        {
            unsigned aux1 = peek(s, ICAX1Z);
            unsigned aux2 = peek(s, ICAX2Z);
            int unit      = peek(s, ICDNOZ);

            sim65_dprintf(s, "RDEV: OPEN channel %d, unit %d, aux1=$%02x, aux2=$%02x",
                          channel, unit, aux1, aux2);

            if (ch->is_open)
            {
                regs->y = 0x81; // Device already open
                return 0;
            }

            // Configure baud rate from AUX1
            ch->baud_rate   = get_baud_rate(aux1);
            ch->translation = (aux2 >> 4) & 0x03; // Translation mode in upper nibble

            // For now, use stdin/stdout
            ch->input_file  = NULL; // stdin
            ch->output_file = NULL; // stdout

            ch->is_open = 1;
            regs->y    = 1; // Success
            sim65_dprintf(s, "RDEV: opened with baud rate %d", ch->baud_rate);
            return 0;
        }

        case DEVR_CLOSE:
        {
            sim65_dprintf(s, "RDEV: CLOSE channel %d", channel);
            if (ch->input_file && ch->input_file != stdin)
                fclose(ch->input_file);
            if (ch->output_file && ch->output_file != stdout)
                fclose(ch->output_file);
            memset(ch, 0, sizeof(*ch));
            regs->y = 1; // Success
            return 0;
        }

        case DEVR_GET:
        {
            if (!ch->is_open)
            {
                regs->y = 0x83; // Channel not open
                return 0;
            }

            // Try to get byte from POKEY serial input
            uint8_t byte;
            if (atari_serial_get_byte(&byte))
            {
                regs->a = byte;
                regs->y = 1; // Success
            }
            else
            {
                // No data available - try reading from input file
                int c;
                if (ch->input_file)
                {
                    c = fgetc(ch->input_file);
                }
                else
                {
                    // Use callback if available
                    extern int (*atari_get_char)(void);
                    if (atari_get_char)
                        c = atari_get_char();
                    else
                        c = EOF;
                }

                if (c == EOF)
                {
                    regs->y = 136; // End of file
                }
                else
                {
                    regs->a = c & 0xFF;
                    regs->y = 1; // Success
                    // Also put it in POKEY buffer for SERIN access
                    atari_serial_put_byte(regs->a);
                }
            }
            return 0;
        }

        case DEVR_PUT:
        {
            if (!ch->is_open)
            {
                regs->y = 0x83; // Channel not open
                return 0;
            }

            uint8_t byte = regs->a;

            // Write to output file
            if (ch->output_file)
            {
                fputc(byte, ch->output_file);
                fflush(ch->output_file);
            }
            else
            {
                // Use callback if available
                extern void (*atari_put_char)(int);
                if (atari_put_char)
                    atari_put_char(byte);
                else
                    putchar(byte);
                fflush(stdout);
            }

            // Also send to POKEY SEROUT (which will call atari_serial_put_byte)
            // This creates a loop: R: -> POKEY -> R: -> output
            // For now, we'll just output directly

            regs->y = 1; // Success
            return 0;
        }

        case DEVR_STATUS:
        {
            if (!ch->is_open)
            {
                regs->y = 0x83; // Channel not open
                return 0;
            }

            // Return status in DVSTAT (0x02EA-0x02ED)
            // Format: [error flags, input level low, input level high, output level]
            unsigned dvstat = 0x02EA;
            uint8_t status[4] = {0, 0, 0, 0};

            // Check for errors (framing, overrun, etc.) - simplified for now
            status[0] = 0; // No errors

            // Input/output buffer levels (simplified)
            status[1] = 0; // Input level low
            status[2] = 0; // Input level high
            status[3] = 0; // Output level

            sim65_add_data_ram(s, dvstat, status, 4);
            regs->y = 1; // Success
            return 0;
        }

        case DEVR_SPECIAL:
        {
            if (!ch->is_open)
            {
                regs->y = 0x83; // Channel not open
                return 0;
            }

            unsigned cmd = peek(s, ICCOMZ);
            sim65_dprintf(s, "RDEV: SPECIAL command $%02x on channel %d", cmd, channel);

            // Handle XIO commands
            // XIO 32: Force short block
            // XIO 34: Set control lines
            // XIO 36: Set baud rate, word size, stop bits
            // XIO 38: Set translation modes and parity
            // XIO 40: Start concurrent mode I/O

            switch (cmd)
            {
                case 36: // Set baud rate
                {
                    unsigned aux1 = peek(s, ICAX1Z);
                    ch->baud_rate = get_baud_rate(aux1);
                    sim65_dprintf(s, "RDEV: baud rate set to %d", ch->baud_rate);
                    regs->y = 1;
                    break;
                }
                case 38: // Set translation mode
                {
                    unsigned aux1 = peek(s, ICAX1Z);
                    ch->translation = (aux1 >> 4) & 0x03;
                    sim65_dprintf(s, "RDEV: translation mode set to %d", ch->translation);
                    regs->y = 1;
                    break;
                }
                default:
                    sim65_dprintf(s, "RDEV: unsupported SPECIAL command $%02x", cmd);
                    regs->y = 1; // Success (ignore unsupported commands)
                    break;
            }
            return 0;
        }

        case DEVR_INIT:
            sim65_dprintf(s, "RDEV: INIT");
            return 0;

        default:
            return cb_error(s, addr);
    }
}

void atari_rdev_init(sim65 s)
{
    // Initialize all channels
    memset(rdev_channels, 0, sizeof(rdev_channels));

    // Register handler callbacks
    add_rts_callback(s, RDEV_BASE, 8, sim_RDEV);
}

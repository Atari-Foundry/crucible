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
#include "hw.h"
#include "atari.h"
#include "sim65.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static int sim_exec_error(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    sim65_eprintf(s, "invalid exec address $%04x", addr);
    return -1;
}

static int sim_gtia(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    // addr & 0x1F
    int reg = addr & 0x1F;
    if (data == sim65_cb_read)
    {
        switch (reg)
        {
            case 0x00: // HPOSM0 ($D000) - Player 0 horizontal position
            case 0x01: // HPOSM1 ($D001) - Player 1 horizontal position
            case 0x02: // HPOSM2 ($D002) - Player 2 horizontal position
            case 0x03: // HPOSM3 ($D003) - Player 3 horizontal position
            case 0x04: // HPOSM0 ($D004) - Missile 0 horizontal position
            case 0x05: // HPOSM1 ($D005) - Missile 1 horizontal position
            case 0x06: // HPOSM2 ($D006) - Missile 2 horizontal position
            case 0x07: // HPOSM3 ($D007) - Missile 3 horizontal position
            case 0x08: // SIZEM ($D008) - Missile size
            case 0x09: // SIZEP0 ($D009) - Player 0 size
            case 0x0A: // SIZEP1 ($D00A) - Player 1 size
            case 0x0B: // SIZEP2 ($D00B) - Player 2 size
            case 0x0C: // SIZEP3 ($D00C) - Player 3 size
            case 0x0D: // GRAFM ($D00D) - Missile graphics
            case 0x0E: // GRAFP0 ($D00E) - Player 0 graphics
            case 0x0F: // GRAFP1 ($D00F) - Player 1 graphics
            case 0x10: // GRAFP2 ($D010) - Player 2 graphics
            case 0x11: // GRAFP3 ($D011) - Player 3 graphics
            case 0x12: // COLPM0 ($D012) - Player/Missile 0 color
            case 0x13: // COLPM1 ($D013) - Player/Missile 1 color
            case 0x14: // COLPM2 ($D014) - Player/Missile 2 color
            case 0x15: // COLPM3 ($D015) - Player/Missile 3 color
            case 0x16: // COLPF0 ($D016) - Playfield 0 color
            case 0x17: // COLPF1 ($D017) - Playfield 1 color
            case 0x18: // COLPF2 ($D018) - Playfield 2 color
            case 0x19: // COLPF3 ($D019) - Playfield 3 color
            case 0x1A: // COLBK ($D01A) - Background color
            case 0x1B: // PRIOR ($D01B) - Priority/GTIA mode
            case 0x1C: // VDELAY ($D01C) - Vertical delay
            case 0x1D: // GRACTL ($D01D) - Graphics control
            case 0x1E: // HITCLR ($D01E) - Hit clear
                // Return 0 for most registers (not implemented yet)
                return 0;
            case 0x1F: // CONSOL ($D01F) - Console keys
                return 7; // No key pressed
            default:
                sim65_dprintf(s, "GTIA read $%04x", addr);
                return 0;
        }
    }
    else
    {
        // Write operations - just log for now
        switch (reg)
        {
            case 0x1F: // CONSOL - read-only
                break;
            default:
                // Don't log zero writes
                if (data != 0)
                    sim65_dprintf(s, "GTIA write $%04x <- $%02x", addr, data);
                break;
        }
    }
    return 0;
}

static int rand32()
{
    static uint32_t a, b, c, d, seed;
    if (!seed)
    {
        a = 0xf1ea5eed, b = c = d = seed = 123;
    }
    uint32_t e;
    e = a - ((b << 27) | (b >> 5));
    a = b ^ ((c << 17) | (c >> 15));
    b = c + d;
    c = d + e;
    d = e + a;
    return d;
}

// Serial I/O state for POKEY
#define SERIAL_BUFFER_SIZE 256
struct pokey_serial_state
{
    // Input buffer (FIFO)
    uint8_t input_buffer[SERIAL_BUFFER_SIZE];
    int input_head;
    int input_tail;
    int input_count;

    // Output buffer (FIFO)
    uint8_t output_buffer[SERIAL_BUFFER_SIZE];
    int output_head;
    int output_tail;
    int output_count;

    // Control registers
    uint8_t skctl;  // Serial port control
    uint8_t skstat; // Serial port status
    uint8_t irqen;  // Interrupt enable
    uint8_t irqst;  // Interrupt status

    // Serial data in flip (toggles on each bit)
    int serial_data_in_flip;

    // Audio/Timer registers
    uint8_t audf[4];   // Audio frequency registers ($D200, $D202, $D204, $D206)
    uint8_t audc[4];   // Audio control registers ($D201, $D203, $D205, $D207)
    uint8_t audctl;    // Audio control ($D208)

    // Keyboard register
    uint8_t kbcode;    // Keyboard code ($D209)

    // Potentiometer registers (read-only)
    uint8_t pot[8];    // Potentiometer inputs ($D200-$D207 when reading)
};

static struct pokey_serial_state serial_state = {0};

// Functions for R: device integration
void atari_serial_put_byte(uint8_t byte)
{
    // This will be called by R: device to send data to POKEY
    // For now, just add to input buffer
    if (serial_state.input_count < SERIAL_BUFFER_SIZE)
    {
        serial_state.input_buffer[serial_state.input_head] = byte;
        serial_state.input_head = (serial_state.input_head + 1) % SERIAL_BUFFER_SIZE;
        serial_state.input_count++;
        // Set input ready bit
        serial_state.skstat |= 0x40; // Set bit 6 (input ready)
        // Toggle serial data in flip
        serial_state.serial_data_in_flip = !serial_state.serial_data_in_flip;
        // Trigger input ready interrupt if enabled
        if (serial_state.irqen & 0x20)
        {
            serial_state.irqst |= 0x20; // Set bit 5 (input ready interrupt)
        }
    }
    else
    {
        // Buffer overrun - set framing error
        serial_state.skstat |= 0x80; // Set bit 7 (framing error)
    }
}

int atari_serial_get_byte(uint8_t *byte)
{
    // This will be called by R: device to get data from POKEY output buffer
    if (serial_state.output_count > 0)
    {
        *byte = serial_state.output_buffer[serial_state.output_tail];
        serial_state.output_tail = (serial_state.output_tail + 1) % SERIAL_BUFFER_SIZE;
        serial_state.output_count--;
        // Clear output ready bit if buffer is empty
        if (serial_state.output_count == 0)
        {
            serial_state.skstat &= ~0x20; // Clear bit 5 (output ready)
            serial_state.irqst &= ~0x10;    // Clear bit 4 (output ready interrupt)
        }
        return 1;
    }
    return 0;
}

static int sim_pokey(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    int reg = addr & 0x0F;

    if (data == sim65_cb_read)
    {
        switch (reg)
        {
            case 0x00: // AUDF1 or POT0 ($D200)
            case 0x01: // AUDC1 or POT1 ($D201)
            case 0x02: // AUDF2 or POT2 ($D202)
            case 0x03: // AUDC2 or POT3 ($D203)
            case 0x04: // AUDF3 or POT4 ($D204)
            case 0x05: // AUDC3 or POT5 ($D205)
            case 0x06: // AUDF4 or POT6 ($D206)
            case 0x07: // AUDC4 or POT7 ($D207)
                // When reading, these are potentiometer inputs
                // Return 0xFF (no input) for now
                return 0xFF;
            case 0x08: // AUDCTL ($D208)
                return serial_state.audctl;
            case 0x09: // KBCODE ($D209) - Keyboard code
                return serial_state.kbcode;
            case 0x0A: // RANDOM/SKRES ($D20A)
                return 0xFF & rand32();
            case 0x0D: // SERIN ($D20D)
            {
                uint8_t byte;
                if (serial_state.input_count > 0)
                {
                    byte = serial_state.input_buffer[serial_state.input_tail];
                    serial_state.input_tail = (serial_state.input_tail + 1) % SERIAL_BUFFER_SIZE;
                    serial_state.input_count--;
                    // Clear input ready bit if buffer is empty
                    if (serial_state.input_count == 0)
                    {
                        serial_state.skstat &= ~0x40; // Clear bit 6 (input ready)
                        serial_state.irqst &= ~0x20;  // Clear bit 5 (input ready interrupt)
                    }
                    return byte;
                }
                return 0; // No data available
            }
            case 0x0E: // IRQST ($D20E)
                return serial_state.irqst;
            case 0x0F: // SKSTAT ($D20F)
            {
                uint8_t status = serial_state.skstat;
                // Toggle serial data in bit (bit 4) for compatibility
                if (serial_state.serial_data_in_flip)
                {
                    status ^= 0x10;
                }
                return status;
            }
            default:
                sim65_dprintf(s, "POKEY read $%04x", addr);
                return 0;
        }
    }
    else
    {
        switch (reg)
        {
            case 0x00: // AUDF1 ($D200)
                serial_state.audf[0] = data;
                sim65_dprintf(s, "POKEY AUDF1 <- $%02x", data);
                break;
            case 0x01: // AUDC1 ($D201)
                serial_state.audc[0] = data;
                sim65_dprintf(s, "POKEY AUDC1 <- $%02x", data);
                break;
            case 0x02: // AUDF2 ($D202)
                serial_state.audf[1] = data;
                sim65_dprintf(s, "POKEY AUDF2 <- $%02x", data);
                break;
            case 0x03: // AUDC2 ($D203)
                serial_state.audc[1] = data;
                sim65_dprintf(s, "POKEY AUDC2 <- $%02x", data);
                break;
            case 0x04: // AUDF3 ($D204)
                serial_state.audf[2] = data;
                sim65_dprintf(s, "POKEY AUDF3 <- $%02x", data);
                break;
            case 0x05: // AUDC3 ($D205)
                serial_state.audc[2] = data;
                sim65_dprintf(s, "POKEY AUDC3 <- $%02x", data);
                break;
            case 0x06: // AUDF4 ($D206)
                serial_state.audf[3] = data;
                sim65_dprintf(s, "POKEY AUDF4 <- $%02x", data);
                break;
            case 0x07: // AUDC4 ($D207)
                serial_state.audc[3] = data;
                sim65_dprintf(s, "POKEY AUDC4 <- $%02x", data);
                break;
            case 0x08: // AUDCTL ($D208)
                serial_state.audctl = data;
                sim65_dprintf(s, "POKEY AUDCTL <- $%02x", data);
                break;
            case 0x0D: // SEROUT ($D20D)
            {
                // Write byte to output buffer
                if (serial_state.output_count < SERIAL_BUFFER_SIZE)
                {
                    serial_state.output_buffer[serial_state.output_head] = data;
                    serial_state.output_head =
                        (serial_state.output_head + 1) % SERIAL_BUFFER_SIZE;
                    serial_state.output_count++;
                    // Set output ready bit
                    serial_state.skstat |= 0x20; // Set bit 5 (output ready)
                    // Trigger output ready interrupt if enabled
                    if (serial_state.irqen & 0x10)
                    {
                        serial_state.irqst |= 0x10; // Set bit 4 (output ready interrupt)
                    }
                    // Send to R: device if available
                    atari_serial_put_byte(data);
                }
                else
                {
                    // Buffer overrun - set framing error
                    serial_state.skstat |= 0x80; // Set bit 7 (framing error)
                }
                break;
            }
            case 0x0E: // IRQEN ($D20E)
            {
                serial_state.irqen = data;
                // Clear interrupt bits that are no longer enabled
                serial_state.irqst &= data;
                // Update interrupt status based on current state
                if (data & 0x10) // Output ready interrupt enabled
                {
                    if (serial_state.output_count > 0)
                        serial_state.irqst |= 0x10;
                }
                if (data & 0x20) // Input ready interrupt enabled
                {
                    if (serial_state.input_count > 0)
                        serial_state.irqst |= 0x20;
                }
                break;
            }
            case 0x0F: // SKCTL ($D20F)
            {
                serial_state.skctl = data;
                // Reset serial status when SKCTL is written
                serial_state.skstat = 0;
                serial_state.serial_data_in_flip = 0;
                sim65_dprintf(s, "POKEY SKCTL <- $%02x", data);
                break;
            }
            default:
                // Don't log zero writes
                if (data != 0)
                    sim65_dprintf(s, "POKEY write $%04x <- $%02x", addr, data);
                break;
        }
    }
    return 0;
}

static int sim_pia(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    int reg = addr & 0x03;
    // PIA registers: $D300-$D303
    // reg 0: PORTA/PACTL ($D300/$D302)
    // reg 1: PORTB/PBCTL ($D301/$D303)
    // reg 2: PACTL (when accessing $D302)
    // reg 3: PBCTL (when accessing $D303)
    
    // Implement 128K memory (Atari 130XE):
    if (reg == 0x01) // PORTB ($D301)
    {
        // This does not emulate PBCTL, assuming the direction bits are
        // correctly setup.
        static uint8_t state = 0xFF;
        if (data == sim65_cb_read)
            return state;
        else
        {
            if (!(data & 0x1))
                sim65_dprintf(s, "PIA: ROM banking not implemented");
            int pre_bank = (state & 0x10) ? 1 : 4 + ((state >> 2) & 3);
            int new_bank = (data & 0x10) ? 1 : 4 + ((data >> 2) & 3);
            if (pre_bank != new_bank)
            {
                sim65_dprintf(s, "PIA: setting bank %d from %d", new_bank, pre_bank);
                // Swap out old bank, swap in new bank
                sim65_swap_bank(s, 0x4000, pre_bank * 0x4000, 0x4000);
                sim65_swap_bank(s, 0x4000, new_bank * 0x4000, 0x4000);
            }
            state = data;
            return 0;
        }
    }
    else if (reg == 0x00) // PORTA ($D300)
    {
        // PORTA - joystick/paddle inputs
        static uint8_t porta_state = 0xFF;
        if (data == sim65_cb_read)
            return porta_state; // Return all bits high (no input)
        else
        {
            // Write to PORTA (direction control)
            porta_state = data;
            return 0;
        }
    }
    else if (reg == 0x02 || reg == 0x03) // PACTL/PBCTL ($D302/$D303)
    {
        // Control registers - basic implementation
        if (data == sim65_cb_read)
            return 0x3C; // Default control register value
        else
        {
            // Write to control register
            sim65_dprintf(s, "PIA %s <- $%02x", (reg == 0x02) ? "PACTL" : "PBCTL", data);
            return 0;
        }
    }
    
    if (data == sim65_cb_read)
        sim65_dprintf(s, "PIA read $%04x", addr);
    else
        sim65_dprintf(s, "PIA write $%04x <- $%02x", addr, data);
    return 0;
}

// Get the vcount counter
static int64_t atari_hw_vcount(sim65 s, int flags)
{
    if (flags & atari_opt_cycletime)
    {
        // Each scan line has 114 cycles, with 9 cycles for refresh, so we
        // simulate 105 cycles/frame.
        int64_t scanlines = sim65_get_cycles(s) / 105;

        // In NTSC, there are 262 scan lines per frame, in PAL there are
        // 312 scan lines per frame, the scan line counter only counts even
        // lines.
        return scanlines / 2;
    }
    else
    {
        // Get current time in seconds:
        struct timeval tv;
        gettimeofday(&tv, 0);
        double time = (tv.tv_sec + tv.tv_usec * 0.000001);
        // Scale depending on PAL/NTSC setting
        if (flags & atari_opt_pal)
            time = time * (15556.55 * 0.5); // PAL
        else
            time = time * (15699.75 * 0.5); // NTSC
        if (sizeof(long) == sizeof(int64_t))
            return lrint(time);
        else
            return llrint(time);
    }
}

// Get the frame counter
int64_t atari_hw_framenum(sim65 s)
{
    int flags      = atari_get_flags(s);
    int64_t vcount = atari_hw_vcount(s, flags);

    if (flags & atari_opt_pal)
        return vcount / 156;
    else
        return vcount / 131;
}

static int sim_antic(sim65 s, struct sim65_reg *regs, unsigned addr, int data)
{
    // addr & 0x0F
    int reg = addr & 0x0F;
    if (data == sim65_cb_read)
    {
        switch (reg)
        {
            case 0x00: // DMACTL ($D400) - DMA control
            case 0x01: // CHACTL ($D401) - Character control
            case 0x02: // DLISTL ($D402) - Display list low
            case 0x03: // DLISTH ($D403) - Display list high
            case 0x04: // HSCROL ($D404) - Horizontal scroll
            case 0x05: // VSCROL ($D405) - Vertical scroll
            case 0x06: // PMBASE ($D406) - Player/Missile base
            case 0x07: // CHBASE ($D407) - Character base
            case 0x08: // WSYNC ($D408) - Wait for sync
            case 0x09: // VCOUNT ($D409) - Vertical count (read-only)
            case 0x0A: // PENH ($D40A) - Light pen horizontal
            case 0x0B: // PENV ($D40B) - Light pen vertical
            case 0x0C: // NMIEN ($D40C) - NMI enable
            case 0x0D: // NMIST ($D40D) - NMI status
            case 0x0E: // NMIRES ($D40E) - NMI reset
                if (reg == 0x0B) // VCOUNT
                {
                    int flags      = atari_get_flags(s);
                    int64_t vcount = atari_hw_vcount(s, flags);
                    if (flags & atari_opt_pal)
                        return vcount % 156;
                    else
                        return vcount % 131;
                }
                // Other registers return 0 for now
                return 0;
            default:
                sim65_dprintf(s, "ANTIC read $%04x", addr);
                return 0;
        }
    }
    else
    {
        // Write operations - just log for now
        switch (reg)
        {
            case 0x08: // WSYNC - write-only, triggers wait
            case 0x09: // VCOUNT - read-only
            case 0x0D: // NMIST - read-only
            case 0x0E: // NMIRES - write-only, clears NMI
                break;
            default:
                // Don't log zero writes
                if (data != 0)
                    sim65_dprintf(s, "ANTIC write $%04x <- $%02x", addr, data);
                break;
        }
    }
    return 0;
}

void atari_hardware_init(sim65 s)
{
    // HW registers
    sim65_add_callback_range(s, 0xD000, 0x100, sim_gtia, sim65_cb_read);
    sim65_add_callback_range(s, 0xD200, 0x100, sim_pokey, sim65_cb_read);
    sim65_add_callback_range(s, 0xD300, 0x100, sim_pia, sim65_cb_read);
    sim65_add_callback_range(s, 0xD400, 0x100, sim_antic, sim65_cb_read);
    sim65_add_callback_range(s, 0xD000, 0x100, sim_gtia, sim65_cb_write);
    sim65_add_callback_range(s, 0xD200, 0x100, sim_pokey, sim65_cb_write);
    sim65_add_callback_range(s, 0xD300, 0x100, sim_pia, sim65_cb_write);
    sim65_add_callback_range(s, 0xD400, 0x100, sim_antic, sim65_cb_write);
    // Error out on EXEC to HW range
    sim65_add_callback_range(s, 0xD000, 0x7FF, sim_exec_error, sim65_cb_exec);
}

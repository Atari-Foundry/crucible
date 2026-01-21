#!/usr/bin/env python3
"""
Simple XEX file creator for Atari 8-bit
XEX format: $FF $FF <start_low> <start_high> <end_low> <end_high> <data...> $FF $FF
"""

import sys

def create_xex(filename, start_addr, data, run_addr=None):
    """Create a XEX file from data"""
    with open(filename, 'wb') as f:
        # XEX header
        f.write(b'\xFF\xFF')
        
        # Start address (little-endian)
        f.write(bytes([start_addr & 0xFF, (start_addr >> 8) & 0xFF]))
        
        # End address (little-endian)
        end_addr = start_addr + len(data) - 1
        f.write(bytes([end_addr & 0xFF, (end_addr >> 8) & 0xFF]))
        
        # Data
        f.write(data)
        
        # If run address specified, add another segment
        if run_addr is not None and run_addr != start_addr:
            f.write(b'\xFF\xFF')
            # RUNAD segment: $FF $FF $E0 $02 <run_low> <run_high>
            f.write(b'\xFF\xFF')
            f.write(b'\xE0\x02')  # RUNAD address
            f.write(bytes([run_addr & 0xFF, (run_addr >> 8) & 0xFF]))
            f.write(b'\x00')  # End of RUNAD segment
        
        # End marker
        f.write(b'\xFF\xFF')

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: create_xex.py <output.xex> <start_addr> <run_addr> <hex_data>")
        print("Example: create_xex.py test.xex 0x600 0x600 'A9 48 20 C9 EF 60'")
        sys.exit(1)
    
    filename = sys.argv[1]
    start_addr = int(sys.argv[2], 0)
    run_addr = int(sys.argv[3], 0)
    hex_data = sys.argv[4].replace(' ', '')
    
    data = bytes.fromhex(hex_data)
    create_xex(filename, start_addr, data, run_addr)
    print(f"Created {filename}")

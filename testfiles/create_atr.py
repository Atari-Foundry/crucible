#!/usr/bin/env python3
"""
Create a simple ATR disk image for testing
ATR format:
- 16-byte header:
  - Bytes 0-1: Magic ($0296)
  - Bytes 2-3: Paragraph size (128 = 0x0080, 256 = 0x0100)
  - Bytes 4-7: Sector count
  - Bytes 8-15: Unused/reserved
- Then sector data
"""

import struct
import sys

def create_simple_atr(filename, sectors=720, sec_size=128):
    """Create a simple ATR image"""
    # ATR header format (from atsio.c):
    # Bytes 0-1: Magic ($0296)
    # Bytes 2-3: Low/mid bytes of image size (paragraph size)
    # Bytes 4-5: Sector size (128 or 256)
    # Bytes 6-7: High byte of image size
    # Bytes 8-15: Reserved/zero
    # Image size calculation: (hdr[2] << 4) | (hdr[3] << 12) | (hdr[6] << 20)
    
    image_size = sectors * sec_size
    
    # ATR header
    header = bytearray(16)
    header[0:2] = struct.pack('<H', 0x0296)  # Magic
    header[4:6] = struct.pack('<H', sec_size)  # Sector size
    # Image size: (hdr[2] << 4) | (hdr[3] << 12) | (hdr[6] << 20)
    # So: hdr[2] = (image_size >> 4) & 0xFF
    #     hdr[3] = (image_size >> 12) & 0xFF
    #     hdr[6] = (image_size >> 20) & 0xFF
    header[2] = (image_size >> 4) & 0xFF
    header[3] = (image_size >> 12) & 0xFF
    header[6] = (image_size >> 20) & 0xFF
    header[7] = 0  # Not used in calculation
    # Bytes 8-15 are reserved/zero
    
    # Create empty sectors
    data = bytearray(sectors * sec_size)
    
    # Write a simple test file in DOS 2.x format
    # This is very simplified - just put some data in sector 361 (first directory sector)
    # For a real DOS, we'd need VTOC, directory entries, etc.
    # For now, just create an empty but valid ATR
    
    with open(filename, 'wb') as f:
        f.write(header)
        f.write(data)
    
    print(f"Created {filename} ({sectors} sectors, {sec_size} bytes/sector, {image_size} bytes)")

if __name__ == '__main__':
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    else:
        filename = 'test.atr'
    
    create_simple_atr(filename)

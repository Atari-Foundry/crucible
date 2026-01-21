# Crucible Test Files

This directory contains test programs and files for testing the crucible Atari 8-bit emulator.

## Test Programs

### hello.xex
Simple "HELLO" program that uses CIO to print to the screen.

Run with:
```bash
./build/crucible testfiles/hello.xex
```

### read_h.xex
Program that reads a file from the H: device (host filesystem).

Run with:
```bash
./build/crucible -R testfiles testfiles/read_h.xex
```

## Test Files

### TEST.TXT
Test file for H: device access. Contains "Hello from H: device".

### test.atr
Simple ATR disk image for testing D: device access.

Run with:
```bash
./build/crucible -I testfiles/test.atr
```

## Tools

### create_xex.py
Python script to create XEX format files from hex data.

Usage:
```bash
python3 create_xex.py <output.xex> <start_addr> <run_addr> <hex_data>
```

### create_simple_xex.py
Creates simple test XEX programs (hello.xex, read_h.xex).

### create_atr.py
Creates simple ATR disk images.

Usage:
```bash
python3 create_atr.py [filename]
```

## Assembly Source Files

### test_d_device.asm
Assembly source for testing D: device (ATR filesystem) access.

### test_h_device.asm
Assembly source for testing H: device (host filesystem) access.

### test_dir.asm
Assembly source for testing directory listing.

Note: These assembly files require ca65 to assemble into XEX format.

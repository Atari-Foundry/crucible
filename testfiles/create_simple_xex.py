#!/usr/bin/env python3
"""
Create simple XEX files for testing
"""

def create_hello_xex():
    """Create a simple hello world program"""
    # Simple program that prints "HELLO" using CIO PUT CHAR
    # IOCB #0 should be initialized for E: device
    # PUT CHAR command is 11 ($0B)
    # Character must be in CIOCHR ($2F) before calling CIOV
    # IOCB #0 ICCOM is at $0342
    
    # Program at $600:
    #   LDX #0        A2 00  (IOCB #0)
    #   LDA #11       A9 0B  (PUT CHAR command)
    #   STA $0342     STA ICCOM (IOCB #0)
    #   LDA #'H'      A9 48
    #   STA $2F       STA CIOCHR
    #   JSR $E456     JSR CIOV
    #   LDA #'E'      A9 45
    #   STA $2F       STA CIOCHR
    #   JSR $E456     JSR CIOV
    #   ... (repeat for L, L, O)
    #   RTS           60
    
    code = bytes([
        # Print 'H' using IOCB #0, E: device
        # IOCB #0 is at $0340, ICCOM is at $0342
        # We need to store directly to $0342 since ICCOM is absolute address
        0xA9, 0x0B,  # LDA #11 (PUT CHAR command)
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM for IOCB #0)
        0xA9, 0x48,  # LDA #'H'
        0x8D, 0x2F, 0x00,  # STA $002F (CIOCHR)
        0xA2, 0x00,  # LDX #0 (IOCB #0 for CIOV)
        0x20, 0x56, 0xE4,  # JSR $E456 (CIOV)
        
        # Print 'E'
        0xA9, 0x0B,  # LDA #11 (PUT CHAR)
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM)
        0xA9, 0x45,  # LDA #'E'
        0x8D, 0x2F, 0x00,  # STA CIOCHR
        0xA2, 0x00,  # LDX #0
        0x20, 0x56, 0xE4,  # JSR CIOV
        
        # Print 'L'
        0xA9, 0x0B,  # LDA #11
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM)
        0xA9, 0x4C,  # LDA #'L'
        0x8D, 0x2F, 0x00,  # STA CIOCHR
        0xA2, 0x00,  # LDX #0
        0x20, 0x56, 0xE4,  # JSR CIOV
        
        # Print 'L'
        0xA9, 0x0B,  # LDA #11
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM)
        0xA9, 0x4C,  # LDA #'L'
        0x8D, 0x2F, 0x00,  # STA CIOCHR
        0xA2, 0x00,  # LDX #0
        0x20, 0x56, 0xE4,  # JSR CIOV
        
        # Print 'O'
        0xA9, 0x0B,  # LDA #11
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM)
        0xA9, 0x4F,  # LDA #'O'
        0x8D, 0x2F, 0x00,  # STA CIOCHR
        0xA2, 0x00,  # LDX #0
        0x20, 0x56, 0xE4,  # JSR CIOV
        
        # Print EOL
        0xA9, 0x0B,  # LDA #11
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM)
        0xA9, 0x0D,  # LDA #$0D (EOL)
        0x8D, 0x2F, 0x00,  # STA CIOCHR
        0xA2, 0x00,  # LDX #0
        0x20, 0x56, 0xE4,  # JSR CIOV
        
        0x60,        # RTS
    ])
    
    with open('hello.xex', 'wb') as f:
        f.write(b'\xFF\xFF')
        f.write(bytes([0x00, 0x06]))  # Start: $0600
        f.write(bytes([0x4A, 0x06]))  # End: $064A (updated for longer code)
        f.write(code)
        f.write(b'\xFF\xFF')
    
    print("Created hello.xex")

def create_read_h_test():
    """Create a program that reads from H: device"""
    # Program to open H1:TEST.TXT and read/display it
    # IOCB #1 is at $0350, ICCOM is at $0352
    # Need to set up IOCB properly before OPEN
    code = bytes([
        # Open H1:TEST.TXT (IOCB #1)
        # IOCB #1 is at $0350, ICCOM is at $0352
        # Use absolute addressing since ICCOM is absolute, not indexed
        0xA9, 0x03,  # LDA #3 (OPEN command)
        0x8D, 0x52, 0x03,  # STA $0352 (ICCOM for IOCB #1)
        0xA9, 0x04,  # LDA #4 (read mode)
        0x8D, 0x5A, 0x03,  # STA $035A (ICAX1 for IOCB #1)
        0xA9, 0x00,  # LDA #0
        0x8D, 0x5B, 0x03,  # STA $035B (ICAX2 for IOCB #1)
        0xA9, 0x00,  # LDA #<filename
        0x8D, 0x54, 0x03,  # STA $0354 (ICBAL for IOCB #1)
        0xA9, 0x07,  # LDA #>filename
        0x8D, 0x55, 0x03,  # STA $0355 (ICBAH for IOCB #1)
        0xA2, 0x10,  # LDX #$10 (IOCB #1 for CIOV)
        0x20, 0x56, 0xE4,  # JSR CIOV
        0x30, 0x2A,  # BMI error (branch if error, skip to close)
        
        # Read loop
        # Label: read_loop (at $061A)
        0xA9, 0x07,  # LDA #7 (GET CHAR command)
        0x8D, 0x52, 0x03,  # STA $0352 (ICCOM for IOCB #1)
        0xA2, 0x10,  # LDX #$10 (IOCB #1)
        0x20, 0x56, 0xE4,  # JSR CIOV
        0x30, 0x1A,  # BMI done (EOF, skip to close at $062F)
        
        # Character is now in A register (from CIOV)
        # Output char to screen (IOCB #0, E: device)
        0x8D, 0x2F, 0x00,  # STA $002F (CIOCHR)
        0xA9, 0x0B,  # LDA #11 (PUT CHAR)
        0x8D, 0x42, 0x03,  # STA $0342 (ICCOM for IOCB #0)
        0xA2, 0x00,  # LDX #0 (IOCB #0)
        0x20, 0x56, 0xE4,  # JSR CIOV
        0x4C, 0x1A, 0x06,  # JMP $061A (back to read_loop)
        
        # Close file
        # Label: done (at $062F)
        0xA9, 0x0C,  # LDA #12 (CLOSE)
        0x8D, 0x52, 0x03,  # STA $0352 (ICCOM for IOCB #1)
        0xA2, 0x10,  # LDX #$10 (IOCB #1)
        0x20, 0x56, 0xE4,  # JSR CIOV
        0x60,        # RTS
        
        # Label: error (at $0638)
        0x60,        # RTS (error return)
        
        # Filename at $0700
    ])
    
    filename = b'H1:TEST.TXT' + bytes([0x9B])
    
    with open('read_h.xex', 'wb') as f:
        f.write(b'\xFF\xFF')
        f.write(bytes([0x00, 0x06]))  # Start: $0600
        f.write(bytes([0x3A, 0x06]))  # End: $063A (updated)
        f.write(code)
        # Filename segment
        f.write(b'\xFF\xFF')
        f.write(bytes([0x00, 0x07]))  # Start: $0700
        f.write(bytes([0x0B, 0x07]))  # End: $070B
        f.write(filename)
        f.write(b'\xFF\xFF')
    
    print("Created read_h.xex")

if __name__ == '__main__':
    create_hello_xex()
    create_read_h_test()

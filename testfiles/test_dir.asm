; Simple test program to list directory from D: device
; Uses CIO to open directory and read entries

    .org $600

; CIO vectors
ICCOM = $0342
ICBAL = $0344
ICBAH = $0345
ICBL = $0348
ICBH = $0349
ICAX1 = $034A
ICAX2 = $034B
CIOV = $E456

; Directory name (empty = root)
dirname:
    .byte 'D', '1', ':', $9B

start:
    ; Open directory for read (mode 6)
    lda #6          ; Directory read mode
    sta ICAX1
    lda #0
    sta ICAX2
    
    ; Set directory name
    lda #<dirname
    sta ICBAL
    lda #>dirname
    sta ICBAH
    
    ; Open directory (IOCB #1)
    ldx #$10        ; IOCB #1
    lda #3          ; OPEN command
    sta ICCOM
    jsr CIOV
    
    ; Check for error
    bmi error
    
    ; Read directory entries
read_loop:
    ; Get one character at a time
    ldx #$10        ; IOCB #1
    lda #5          ; GET CHAR command
    sta ICCOM
    jsr CIOV
    
    ; Check for EOF
    bmi done
    
    ; Output character to screen (IOCB #0, E: device)
    ldx #0
    lda #11         ; PUT CHAR
    sta ICCOM
    jsr CIOV
    
    jmp read_loop

done:
    ; Close directory
    ldx #$10
    lda #12         ; CLOSE
    sta ICCOM
    jsr CIOV
    
    rts

error:
    ; Error occurred
    rts

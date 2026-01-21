; Simple test program to read a file from H: device (host filesystem)
; Uses CIO to open and read a file

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

; File name to open
filename:
    .byte 'H', '1', ':', 'T', 'E', 'S', 'T', '.', 'T', 'X', 'T', $9B

start:
    ; Open file for read (mode 4)
    lda #4          ; Open for read
    sta ICAX1
    lda #0
    sta ICAX2
    
    ; Set filename
    lda #<filename
    sta ICBAL
    lda #>filename
    sta ICBAH
    
    ; Open file (IOCB #1)
    ldx #$10        ; IOCB #1
    lda #3          ; OPEN command
    sta ICCOM
    jsr CIOV
    
    ; Check for error
    bmi error
    
    ; Read file
read_loop:
    ldx #$10        ; IOCB #1
    lda #7          ; GET RECORD command
    sta ICCOM
    lda #<buffer
    sta ICBAL
    lda #>buffer
    sta ICBAH
    lda #<80
    sta ICBL
    lda #>80
    sta ICBH
    jsr CIOV
    
    ; Check for EOF
    bmi done
    
    ; Output to screen (IOCB #0, E: device)
    ldx #0
    lda #11         ; PUT RECORD
    sta ICCOM
    jsr CIOV
    
    jmp read_loop

done:
    ; Close file
    ldx #$10
    lda #12         ; CLOSE
    sta ICCOM
    jsr CIOV
    
    rts

error:
    ; Error occurred
    rts

buffer:
    .res 256

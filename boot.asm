ORG 0
BITS 16

_start:
    jmp short start 
    nop

times 33 db 0

start:
    jmp 0x7c0:step2


step2:
    cli ; clear interrupts 
    mov ax, 0x07C0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax    
    mov sp, 0x7C00
    sti ; enable interrupts
    
    mov ah, 2 ; READ SECTOR COMMAND 
    mov al, 1 ; ONE SECTOR TO READ
    mov ch, 0 ; CYLINDER LOW EIGHT BITS 
    mov cl, 2 ; READ SECTOR 2 
    mov dh, 0 ; HEAD NUMBER
    mov bx, buffer ; BUFFER TO READ INTO
    int 0x13
    jc error

    mov si, buffer
    call print 

    jmp $ ; infinite loop
    
error:
    mov si, error_message
    call print 
    jmp $

print:
    mov bx, 0

.loop:    
    lodsb
    cmp al, 0 
    je .done
    call print_char_utf8
    jmp .loop

.done:
    ret

print_char_utf8:
    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    ret

error_message: db 'Failed to load sector', 0


times 510-($-$$) db 0
dw 0xAA55

buffer:

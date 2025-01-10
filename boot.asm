[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000

; 8-bit bootloader initialization
start:
    ; Initialize segments
    cli                     ; Clear interrupts
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti                     ; Enable interrupts

    ; Save boot drive number
    mov [BOOT_DRIVE], dl

    ; Set video mode (80x25 text mode)
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; Clear screen
    mov ah, 0x06
    mov al, 0x00
    mov bh, 0x1F           ; Blue background (1), white text (F)
    mov cx, 0x0000         ; Top left corner
    mov dx, 0x184F         ; Bottom right corner
    int 0x10

    ; Load kernel
    mov bx, KERNEL_OFFSET
    mov dh, 15             ; Number of sectors
    mov dl, [BOOT_DRIVE]
    call load_disk

    ; Jump to kernel
    jmp KERNEL_OFFSET

load_disk:
    push dx
    mov ah, 0x02          ; BIOS read sector function
    mov al, dh            ; Number of sectors
    mov ch, 0x00          ; Cylinder 0
    mov dh, 0x00          ; Head 0
    mov cl, 0x02          ; Start from sector 2
    int 0x13
    jc disk_error
    pop dx
    cmp dh, al
    jne disk_error
    ret

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

; Data
BOOT_DRIVE db 0
DISK_ERROR_MSG db 'Disk read error!', 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55

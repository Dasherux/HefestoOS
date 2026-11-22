[BITS 16]
[ORG 0x7C00]

start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl
	mov [0x0500], dl

    ; Inicializa depuração
    mov si, msg_loading
    call debug_print

    ; Carrega Stage 2
    mov si, disk_packet
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13

    jc disk_error

    mov si, msg_loaded
    call debug_print

    jmp 0x0000:0x8000

disk_error:
    mov si, msg_error
    call debug_print

.halt:
    cli
    hlt
    jmp .halt

debug_print:
    lodsb
    test al, al
    jz .done

    out 0xE9, al
    jmp debug_print

.done:
    ret

boot_drive db 0

msg_loading db "[BOOT] Loading Stage 2...", 10, 0
msg_loaded  db "[BOOT] Stage 2 loaded.", 10, 0
msg_error   db "[BOOT] Disk read error!", 10, 0

align 4

disk_packet:
    db 0x10
    db 0
    dw 32
    dw 0x8000
    dw 0x0000
    dq 1

times 510-($-$$) db 0
dw 0xAA55
[BITS 16]
[ORG 0x8000]

start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax

    mov si, msg_stage2
    call debug_print

    ; Carrega o Kernel em 0x10000
    mov si, msg_kernel_loading
    call debug_print

    mov si, kernel_packet
    mov dl, [0x0500]
    mov ah, 0x42
    int 0x13

    jc disk_error

    mov si, msg_kernel_loaded
    call debug_print

    ; Habilita A20
    in al, 0x92
    or al, 2
    and al, 0xFE
    out 0x92, al

    mov si, msg_a20
    call debug_print

    ; Carrega GDT
    lgdt [gdt_ptr]

    ; Ativa Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:protected_mode


debug_print:
    lodsb
    test al, al
    jz .done

    out 0xE9, al
    jmp debug_print

.done:
    ret


disk_error:
    mov si, msg_error
    call debug_print

.halt:
    cli
    hlt
    jmp .halt


[BITS 32]

protected_mode:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x70000

    ; Copia Kernel: 0x10000 -> 0x100000
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, 15360

    rep movsd

    ; Inicializa estruturas de paginação
    mov edi, 0x90000
    xor eax, eax
    mov ecx, 3072
    rep stosd

    ; PML4 -> PDPT
    mov dword [0x90000], 0x91003

    ; PDPT -> Page Directory
    mov dword [0x91000], 0x92003

    ; Cria 512 páginas de 2 MiB
    mov edi, 0x92000
    mov ebx, 0
    mov ecx, 512

.setup_pages:
    mov eax, ebx
    or eax, 0x83

    mov [edi], eax
    mov dword [edi + 4], 0

    add ebx, 0x200000
    add edi, 8

    loop .setup_pages

    ; Ativa PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Define PML4
    mov eax, 0x90000
    mov cr3, eax

    ; Habilita Long Mode no EFER
    mov ecx, 0xC0000080
    rdmsr

    or eax, 1 << 8

    wrmsr

    ; Ativa paginação
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    mov dx, 0xE9
    mov al, 'P'
    out dx, al

    jmp 0x18:long_mode


[BITS 64]

long_mode:

    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, 0x70000

    ; Confirma Long Mode
    mov rsi, msg_long_mode
    call debug_print64

    ; Transfere execução para o Kernel
    mov rsi, msg_kernel_jump
    call debug_print64

    mov rax, 0x100000
    jmp rax


debug_print64:
    lodsb
    test al, al
    jz .done

    out 0xE9, al
    jmp debug_print64

.done:
    ret


[BITS 16]

msg_stage2 db "[BOOT] Stage 2 initialized.", 10, 0
msg_kernel_loading db "[STAGE2] Loading Kernel...", 10, 0
msg_kernel_loaded db "[STAGE2] Kernel loaded.", 10, 0
msg_a20 db "[KERNEL] A20 enabled.", 10, 0
msg_error db "[STAGE2] Kernel disk read error!", 10, 0
msg_long_mode db "[KERNEL] Long Mode activated.", 10, 0
msg_kernel_jump db "[STAGE2] Jumping to Kernel...", 10, 0

align 4

kernel_packet:
    db 0x10
    db 0
    dw 120
    dw 0
    dw 0x1000
    dq 33


align 8

gdt:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00AF9A000000FFFF
    dq 0x00CF92000000FFFF

gdt_end:

gdt_ptr:
    dw gdt_end - gdt - 1
    dd gdt
	
times 32*512-($-$$) db 0
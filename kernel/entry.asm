[BITS 64]

section .text.boot

global _start
extern kernel_main

extern __bss_start
extern __bss_end

_start:
    cli

    ; Configura stack
    mov rsp, 0x200000
    and rsp, -16

    ; Limpa BSS
    lea rdi, [rel __bss_start]
    lea rcx, [rel __bss_end]

    sub rcx, rdi
    xor eax, eax

    rep stosb

    ; Chama Kernel em C
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
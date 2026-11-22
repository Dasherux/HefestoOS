typedef unsigned char u8;

static void debug_putc(char c) {
    __asm__ volatile ("outb %0, $0xE9"::"a"(c));
}

static void debug_print(const char* str) {
    while (*str) {
        debug_putc(*str++);
    }
}

void kernel_main(void) {
    debug_print("[KERNEL] C Kernel initialized.\n");
    debug_print("[KERNEL] Running in 64-bit mode.\n");
    debug_print("[KERNEL] Boot successful!\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
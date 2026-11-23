#include "../system/shell.h"

void kernel_main(void) {
	shell_main();
 
    while (1) {
        __asm__ volatile ("hlt");
    }
}
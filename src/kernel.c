#include <stdint.h>
// #include <stdio.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/interrupt/idt.h"
#include "header/interrupt/interrupt.h"
#include "header/driver/keyboard.h"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"
#include "header/driver/disk.h"
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"
#include "header/memory/paging.h"



void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    gdt_install_tss();
    set_tss_register();

    paging_allocate_user_page_frame(&_paging_kernel_page_directory, (uint8_t*) 0);

    struct FAT32DriverRequest request = {
        .buf                   = (uint8_t*) 0,
        .name                  = "shell",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0x100000,
    };
    
    read(request);

    set_tss_kernel_current_stack();
    kernel_execute_user_program((uint8_t*) 0);

    while (true);
}
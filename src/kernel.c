#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
// #include "gdt.c"
#include "header/stdlib/string.h"
#include "header/driver/disk.h"
#include "header/filesystem/fat32.h"
#include "header/driver/keyboard.h"
#include "header/interrupt/interrupt.h"
#include "header/interrupt/idt.h"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"
void kernel_setup(void)
{
    // uint32_t a;
    // uint32_t volatile b = 0x0000BABE;
    // __asm__("mov $0xCAFE0000, %0" : "=r"(a));
    // load_gdt(&_gdt_gdtr);
    // framebuffer_clear();
    // framebuffer_write(3, 8,  'H', 0, 0xF);
    // framebuffer_write(3, 9,  'a', 0, 0xF);
    // framebuffer_write(3, 10, 'i', 0, 0xF);
    // framebuffer_write(3, 11, '!', 0, 0xF);
    // framebuffer_set_cursor(3, 10);
    // framebuffer_write(4, 8,  'H', 0, 0xF);
    // framebuffer_write(4, 9,  'a', 0, 0xF);
    // framebuffer_write(4, 10, 'i', 0, 0xF);
    // framebuffer_write(4, 11, '!', 0, 0xF);
    // framebuffer_set_cursor(4, 10);
    // while(true);
    // while (true) b += 1;
    // load_gdt(&_gdt_gdtr);
    // pic_remap();
    // initialize_idt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);
    // framebuffer_write(0, 1, 'h', 0, 14);
    // framebuffer_set_cursor(0,1);
    // __asm__("int $0x4");
    // while (true);
    // load_gdt(&_gdt_gdtr);
    // pic_remap();
    // initialize_idt();
    // activate_keyboard_interrupt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);

    // int col = 0;
    // keyboard_state_activate();
    // while (true) {
    //      char c;
    //      get_keyboard_buffer(&c);
    //      if (c) framebuffer_write(0, col++, c, 0xF, 0);
    // }
    // load_gdt(&_gdt_gdtr);
    // pic_remap();
    // initialize_idt();
    // activate_keyboard_interrupt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);

    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);

    initialize_filesystem_fat32();

    // If read properly, readcbuf should filled with 'a'

    // char cek2 = (char)cek;

    // delete(request);
    // // delete(request);
    // write(request2);

    // struct ClusterBuffer cbuf[5];
    // for (uint32_t i = 0; i < 5; i++)
    // {
    //     for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
    //     {
    //         cbuf[i].buf[j] = i + 'a';
    //     }
    // }

    // struct FAT32DriverRequest request = {
    //     .buf = cbuf,
    //     .name = "oniichan",
    //     .ext = "osu",
    //     .parent_cluster_number = ROOT_CLUSTER_NUMBER,
    //     .buffer_size = 0,
    // };

    // write(request); // Create folder "ikanaide"
    // write_blocks(fs_signature, BOOT_SECTOR, 1);
    // while (true){
    //     keyboard_state_activate();
    // }

    // // int col = 0;
    // while (true)
    // {
    //     //  char c;
    //     //  get_keyboard_buffer(&c);
    //     //  if (c) framebuffer_write(0, col++, c, 0xF, 0);
    //     keyboard_state_activate();
    // }
    // load_gdt(&_gdt_gdtr);
    // pic_remap();
    // activate_keyboard_interrupt();
    // initialize_idt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);

    // struct BlockBuffer b;
    // for (int i = 0; i < 512; i++){
    //     b.buf[i] = i % 16;
    // }
    // write_blocks(&b, 17, 1);
    // while (true);

    while (true)
    {
        keyboard_state_activate();
    }
}
// void kernel_setup(void)
// {
//     load_gdt(&_gdt_gdtr);
//     while (true);
// }

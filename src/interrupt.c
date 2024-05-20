#include "header/interrupt/interrupt.h"
#include "header/cpu/portio.h"
#include "header/driver/keyboard.h"
#include "header/cpu/gdt.h"
#include "header/memory/paging.h"
#include "header/filesystem/fat32.h"
#include "header/text/framebuffer.h"
#include "header/stdlib/string.h"
#include "header/cmos/cmos.h"

void activate_timer_interrupt(void) {
    __asm__ volatile("cli");
    // Setup how often PIT fire
    uint32_t pit_timer_counter_to_fire = PIT_TIMER_COUNTER;
    out(PIT_COMMAND_REGISTER_PIO, PIT_COMMAND_VALUE);
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t) (pit_timer_counter_to_fire & 0xFF));
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t) ((pit_timer_counter_to_fire >> 8) & 0xFF));

    // Activate the interrupt
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_TIMER));
}


void io_wait(void) {
    out(0x80, 0);
}

void pic_ack(uint8_t irq) {
    if (irq >= 8) out(PIC2_COMMAND, PIC_ACK);
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void) {
    // Starts the initialization sequence in cascade mode
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); 
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    out(PIC1_DATA, 0b0100); // ICW3: tell Master PIC, slave PIC at IRQ2 (0000 0100)
    io_wait();
    out(PIC2_DATA, 0b0010); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    // Disable all interrupts
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);
}

void activate_keyboard_interrupt(void) {
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD));
}

void main_interrupt_handler(struct InterruptFrame frame) {
    switch (frame.int_number) {
        case PAGE_FAULT:
            __asm__("hlt");
            break;        
        case IRQ_KEYBOARD + PIC1_OFFSET:
            keyboard_isr();
            break;
        case 0x30:
            syscall(frame);
            break;
    }
}

struct TSSEntry _interrupt_tss_entry = {
    .ss0  = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
};

void set_tss_kernel_current_stack(void) {
    uint32_t stack_ptr;
    // Reading base stack frame instead esp
    __asm__ volatile ("mov %%ebp, %0": "=r"(stack_ptr) : /* <Empty> */);
    // Add 8 because 4 for ret address and other 4 is for stack_ptr variable
    _interrupt_tss_entry.esp0 = stack_ptr + 8; 
}



void syscall(struct InterruptFrame frame) {
    switch (frame.cpu.general.eax) {
        case 0:
            *((int8_t*) frame.cpu.general.ecx) = read(*(struct FAT32DriverRequest*) frame.cpu.general.ebx);
            break;
        case 1:
            struct FAT32DriverRequest request2 = *(struct FAT32DriverRequest *)frame.cpu.general.ebx;
            *((int8_t *)frame.cpu.general.ecx) = read_directory(request2);
            break;
        case 2:
            struct FAT32DriverRequest request = *(struct FAT32DriverRequest *) frame.cpu.general.ebx;
            *((int8_t *)frame.cpu.general.ecx) = write(request);
            break;
        case 4:
            keyboard_state_activate();
            __asm__("sti");
            while (is_keyboard_blocking());
            char buf[256];

            //mengosongkan buf
            for(int i = 0; i < 256; i++){
                buf[i] = '\0';
            }
            
            get_keyboard_buffer(buf);
            memcpy((char*) frame.cpu.general.ebx, buf, frame.cpu.general.ecx);
            break;
        case 5:
            put_char(*((char*)frame.cpu.general.ebx), frame.cpu.general.ecx);
            break;
        case 6:
            puts(
                (char*) frame.cpu.general.ebx, 
                frame.cpu.general.ecx, 
                frame.cpu.general.edx
            ); // Assuming puts() exist in kernel
            break;
        case 7: 
            keyboard_state_activate();
            break;
        case 8:
            framebuffer_clear();
            setZeroLocation();
            framebuffer_set_cursor(0, 0);
            break;
        case 9 :
            while (true)
            {
                struct clock c;
                cmos_read_clock(&c);
                char front_hour_str = { c.front_hour + '0'};
                char behind_hour_str = { c.behind_hour + '0'};
                char front_minute_str = { c.front_minute + '0'};
                char behind_minute_str = { c.behind_minute + '0'};
                char front_second_str = { c.front_second + '0'};
                char behind_second_str = { c.behind_second + '0'};
                framebuffer_write(24, 72, front_hour_str, 0xFF, 0x00);
                framebuffer_write(24, 73, behind_hour_str, 0xFF, 0x00);
                framebuffer_write(24, 74, ':', 0xFF, 0x00);
                framebuffer_write(24, 75, front_minute_str, 0xFF, 0x00);
                framebuffer_write(24, 76, behind_minute_str, 0xFF, 0x00);
                framebuffer_write(24, 77, ':', 0xFF, 0x00);
                framebuffer_write(24, 78, front_second_str, 0xFF, 0x00);
                framebuffer_write(24, 79, behind_second_str, 0xFF, 0x00);
            }
        
            break;
    }
}

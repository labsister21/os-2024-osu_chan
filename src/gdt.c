#include "header/cpu/gdt.h"
#include "header/interrupt/interrupt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        {.segment_low = 0, // Null Descriptor
         .base_low = 0,
         .base_mid = 0,
         .type_bit = 0,
         .non_system = 0,
         .privilege_level = 0,
         .is_present = 0,
         .segment_limit = 0,
         .usable_by_software = 0,
         .is_64bit_segment = 0,
         .default_op_size = 0,
         .granularity = 0,
         .base_high = 0},
        {.segment_low = 0xFFFF, // Kernel Code Descriptor
         .base_low = 0,
         .base_mid = 0,
         .type_bit = 0xA,
         .non_system = 1,
         .privilege_level = 0,
         .is_present = 1,
         .segment_limit = 0xF,
         .usable_by_software = 0,
         .is_64bit_segment = 0,
         .default_op_size = 1,
         .granularity = 1,
         .base_high = 0},
        {.segment_low = 0xFFFF, // Kernel Data Descriptor
         .base_low = 0,
         .base_mid = 0,
         .type_bit = 0x2,
         .non_system = 1,
         .privilege_level = 0,
         .is_present = 1,
         .segment_limit = 0xF,
         .usable_by_software = 0,
         .is_64bit_segment = 0,
         .default_op_size = 1,
         .granularity = 1,
         .base_high = 0},

         {/* TODO: User   Code Descriptor */
        .segment_low = 0xFFFF,
        .base_low = 0x0000,

        .base_mid = 0x00,
        .type_bit = 0xA,
        .non_system = 0x01,
        .privilege_level = 0x03,
        .is_present = 0x01,
        .segment_limit = 0xF,
        .usable_by_software = 0x00,
        .is_64bit_segment = 0x00,
        .default_op_size = 0x01,
        .granularity = 0x01,
        .base_high = 0x00,
        },
        {/* TODO: User   Data Descriptor */
        .segment_low = 0xFFFF,
        .base_low = 0x0000,

        .base_mid = 0x00,
        .type_bit = 0x2,
        .non_system = 0x01,
        .privilege_level = 0x03,
        .is_present = 0x01,
        .segment_limit = 0xF,
        .usable_by_software = 0x00,
        .is_64bit_segment = 0x00,
        .default_op_size = 0x01,
        .granularity = 0x01,
        .base_high = 0x00,
        },
        {
        .segment_limit      = (sizeof(struct TSSEntry) & (0xF << 16)) >> 16,
        .segment_low       = sizeof(struct TSSEntry),
        .base_high         = 0,
        .base_mid          = 0,
        .base_low          = 0,
        .non_system        = 0,    // S bit
        .type_bit          = 0x9,
        .privilege_level   = 0,    // DPL
        .is_present        = 1,    // P bit
        .default_op_size   = 1,    // D/B bit
        .is_64bit_segment  = 0,    // L bit
        .granularity       = 0,    // G bit
        },
        {0}
}};

/**
 * _gdt_gdtr, predefined system GDTR. 
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    // TODO : Implement, this GDTR will point to global_descriptor_table. 
    //        Use sizeof operator
    .size = sizeof(global_descriptor_table),
    .address = &global_descriptor_table,
};

void gdt_install_tss(void) {
    uint32_t base = (uint32_t) &_interrupt_tss_entry;
    global_descriptor_table.table[5].base_high = (base & (0xFF << 24)) >> 24;
    global_descriptor_table.table[5].base_mid  = (base & (0xFF << 16)) >> 16;
    global_descriptor_table.table[5].base_low  = base & 0xFFFF;
}

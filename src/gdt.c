#include "header/cpu/gdt.h"

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

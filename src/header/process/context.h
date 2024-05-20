#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "../interrupt/interrupt.h"
#include "../memory/paging.h"
#include "../process/process.h"

/**
 * Contain information needed for task to be able to get interrupted and resumed later
 *
 * @param cpu                         All CPU register state
 * @param eip                         CPU instruction counter to resume execution
 * @param eflags                      Flag register to load before resuming the execution
 * @param page_directory_virtual_addr CPU register CR3, containing pointer to active page directory
 */

/**
 * Contain information needed for task to be able to get interrupted and resumed later
 *
 * @param cpu                         All CPU register state
 * @param eip                         CPU instruction counter to resume execution
 * @param eflags                      Flag register to load before resuming the execution
 * @param page_directory_virtual_addr CPU register CR3, containing pointer to active page directory
 */
struct Context {
    // TODO: Add important field here
    struct CPURegister cpu;
    uint32_t eip;
    uint32_t eflags;
    struct PageDirectory* page_directory_virtual_addr;
}__attribute__((packed));


typedef enum PROCESS_STATE {
    // TODO: Add process states
    READY,
    BLOCK,
    RUN,
    INACTIVE,
} PROCESS_STATE;

/**
 * Structure data containing information about a process
 *
 * @param metadata Process metadata, contain various information about process
 * @param context  Process context used for context saving & switching
 * @param memory   Memory used for the process
 */
struct ProcessControlBlock {
    struct Context context;
    struct {
        uint32_t pid;
        PROCESS_STATE process_state;
    } metadata;

    struct {
        void     *virtual_addr_used[PROCESS_PAGE_FRAME_COUNT_MAX];
        uint32_t page_frame_used_count;
    } memory;
}__attribute__((packed));


extern struct ProcessControlBlock _process_list[PROCESS_COUNT_MAX];

# endif
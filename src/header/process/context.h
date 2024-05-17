
#include "../interrupt/interrupt.h"
#include "../memory/paging.h"
#include "process.h"

/**
 * Contain information needed for task to be able to get interrupted and resumed later
 *
 * @param cpu                         All CPU register state
 * @param eip                         CPU instruction counter to resume execution
 * @param eflags                      Flag register to load before resuming the execution
 * @param page_directory_virtual_addr CPU register CR3, containing pointer to active page directory
 */
struct Context {
    CPURegister cpu;
    uint32_t eip;
    uint32_t eflags;
    PageDirectory* page_directory_virtual_addr;
};

typedef enum PROCESS_STATE {
    // TODO: Add process states
    READY,
    BLOCK,
    RUN,
    TERMINATE,
    NEW,
} PROCESS_STATE;

/**
 * Structure data containing information about a process
 *
 * @param metadata Process metadata, contain various information about process
 * @param context  Process context used for context saving & switching
 * @param memory   Memory used for the process
 */
struct ProcessControlBlock {
    struct {
        uint32_t pid;
        Context context;
        PROCESS_STATE process_state;
    } metadata;

    struct {
        void     *virtual_addr_used[PROCESS_PAGE_FRAME_COUNT_MAX];
        uint32_t page_frame_used_count;
    } memory;
};

ProcessControlBlock _process_list[PROCESS_COUNT_MAX];


static process_manager_state = {
    .active_process_count = 0,
};
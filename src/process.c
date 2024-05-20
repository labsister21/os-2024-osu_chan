#include "header/memory/paging.h"
#include "header/stdlib/string.h"
#include "header/cpu/gdt.h"
#include "header/interrupt/interrupt.h"
#include "header/math/math.h"
#include "header/process/context.h"


struct ProcessControlBlock _process_list[PROCESS_COUNT_MAX] = {0};
bool isEmptyPCB = true;

static struct {
    int active_process_count;
} 

process_manager_state = {
    .active_process_count = 0,
};

uint32_t process_list_get_inactive_index(){
    int i = 0;    
    for(; i < PROCESS_COUNT_MAX; i++){
        if(_process_list[i].metadata.process_state == INACTIVE || isEmptyPCB){
            isEmptyPCB = false;
            process_manager_state.active_process_count++;
            return i;
        }
    }
    return 17;
}


uint32_t process_generate_new_pid(){
    static int last_assigned_pid = 0;
    return last_assigned_pid++;
}

int32_t process_create_user_process(struct FAT32DriverRequest request) {
    int32_t retcode = PROCESS_CREATE_SUCCESS; 
    if (process_manager_state.active_process_count >= PROCESS_COUNT_MAX) {
        retcode = PROCESS_CREATE_FAIL_MAX_PROCESS_EXCEEDED;
        goto exit_cleanup;
    }

    // Ensure entrypoint is not located at kernel's section at higher half
    if ((uint32_t) request.buf >= KERNEL_VIRTUAL_ADDRESS_BASE) {
        retcode = PROCESS_CREATE_FAIL_INVALID_ENTRYPOINT;
        goto exit_cleanup;
    }

    // Check whether memory is enough for the executable and additional frame for user stack
    uint32_t page_frame_count_needed = ceil_div(request.buffer_size + PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
    if (!paging_allocate_check(page_frame_count_needed) || page_frame_count_needed > PROCESS_PAGE_FRAME_COUNT_MAX) {
        retcode = PROCESS_CREATE_FAIL_NOT_ENOUGH_MEMORY;
        goto exit_cleanup;
    }

    // Process PCB 
    uint32_t p_index = process_list_get_inactive_index();
    struct ProcessControlBlock *new_pcb = &(_process_list[p_index]);

    struct ProcessControlBlock temp = {
    .context = {
        .cpu = {
            .general = {
                .eax = 0,
                .ebx = 0,
                .ecx = 0,
                .edx = 0
            },
            .index = {
                .edi = 0,
                .esi = 0,
            },
            .segment = {
                .ds = 0x23,
                .es = 0x23,
                .fs = 0x23,
                .gs = 0x23,
            },
            .stack = {
                .ebp = 0,
                .esp = 0,
            }
        },
        .eflags = CPU_EFLAGS_BASE_FLAG | CPU_EFLAGS_FLAG_INTERRUPT_ENABLE,
        .eip = 0,

    },
    .metadata = {
        .pid = process_generate_new_pid(),
        .process_state = INACTIVE,
    },
    .memory = {
        .page_frame_used_count = 0,
        },
    };
    *new_pcb = temp;
    new_pcb->context.page_directory_virtual_addr = paging_create_new_page_directory();
    paging_use_page_directory(new_pcb->context.page_directory_virtual_addr);
     paging_allocate_user_page_frame(new_pcb->context.page_directory_virtual_addr, (uint8_t*) 0);
     read(request);

exit_cleanup:
    return retcode;
}
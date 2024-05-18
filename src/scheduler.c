#include "header/scheduler/scheduler.h"
#include "header/process/process.h"
#include "header/interrupt/interrupt.h"
#include "header/memory/paging.h"
#include "header/stdlib/string.h"

// Forward declaration of save_current_context
extern void save_current_context(struct Context* ctx);

// Define the currently running process index
static struct ProcessControlBlock* current_running_process = NULL;
static int current_running_process_index = -1;

/**
 * Initialize scheduler before executing init process 
 */
void scheduler_init(void) {
    // Initialize the process list and activate timer interrupt
    init_process_list();
    activate_timer_interrupt();
}

/**
 * Save context to current running process
 * 
 * @param ctx Context to save to current running process control block
 */
void scheduler_save_context_to_current_running_pcb(struct Context ctx) {
    if (current_running_process != NULL) {
        current_running_process->context = ctx;
    }
}

/**
 * Trigger the scheduler algorithm and context switch to new process
 */
__attribute__((noreturn)) void scheduler_switch_to_next_process(void) {
    int next_process_index = -1;

    // Find the next ready process
    for (int i = 0; i < PROCESS_COUNT_MAX; i++) {
        int index = (current_running_process_index + 1 + i) % PROCESS_COUNT_MAX;
        if (_process_list[index].metadata.process_state == READY) {
            next_process_index = index;
            break;
        }
    }

    if (next_process_index == -1) {
        // If no READY process found, halt the CPU (or handle this situation as needed)
        while (1) {
            __asm__ volatile("hlt");
        }
    }

    current_running_process_index = next_process_index;
    current_running_process = &_process_list[next_process_index];

    current_running_process->metadata.process_state = RUN;
    process_context_switch(current_running_process->context);
}

/**
 * Scheduler ISR handler
 * This function will be called by the timer interrupt
 */
void scheduler_isr_handler(void) {
    // Save the current context
    struct Context current_context;
    save_current_context(&current_context);

    scheduler_save_context_to_current_running_pcb(current_context);

    // Set the current process state to READY
    if (current_running_process != NULL) {
        current_running_process->metadata.process_state = READY;
    }

    // Switch to the next process
    scheduler_switch_to_next_process();
}

// Assembly function to save the current CPU context
extern void save_current_context(struct Context* ctx);

__attribute__((noreturn)) void process_context_switch(struct Context ctx) {
    // Separate block for setting the page directory
    __asm__ volatile (
        "movl %0, %%cr3\n"             // Load page directory
        :
        : "r"(ctx.page_directory_virtual_addr)
        : "memory"
    );

    __asm__ volatile (
        "movl %0, %%ebx\n"             // Load CPU registers
        "movl %1, %%ecx\n"
        "movl %2, %%edx\n"
        "movl %3, %%esi\n"
        "movl %4, %%edi\n"
        "movl %5, %%ebp\n"
        "pushl %6\n"                   // Push eflags
        "pushl %7\n"                   // Push eip
        "iret\n"                       // Jump to eip
        :
        : "r"(ctx.cpu.general.ebx),
          "r"(ctx.cpu.general.ecx),
          "r"(ctx.cpu.general.edx),
          "r"(ctx.cpu.index.esi),
          "r"(ctx.cpu.index.edi),
          "r"(ctx.cpu.stack.ebp),
          "r"(ctx.eflags),
          "r"(ctx.eip)
        : "memory"
    );

    __builtin_unreachable();
}
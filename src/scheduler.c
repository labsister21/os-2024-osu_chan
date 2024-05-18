#include "header/scheduler/scheduler.h"
#include "header/process/process.h"
#include "header/interrupt/interrupt.h"
#include "header/memory/paging.h"
#include "header/stdlib/string.h"

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

// Forward declaration of save_current_context
extern void save_current_context(struct Context* ctx);

// Implementation of save_current_context function
void save_current_context(struct Context* ctx) {

  __asm__  volatile("movl %%ebx, %0" :  "=D" (ctx->cpu.general.ebx) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%edx, %0" :  "=D" (ctx->cpu.general.edx) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%ecx, %0" :  "=D" (ctx->cpu.general.ecx) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%eax, %0" :  "=D" (ctx->cpu.general.eax) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%esi, %0" :  "=D" (ctx->cpu.index.esi) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%edi, %0" :  "=D" (ctx->cpu.index.edi) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%ebp, %0" :  "=D" (ctx->cpu.stack.ebp) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%esp, %0" :  "=D" (ctx->cpu.stack.esp) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%gs, %0" :  "=D" (ctx->cpu.segment.gs) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%fs, %0" :  "=D" (ctx->cpu.segment.fs) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%es, %0" :  "=D" (ctx->cpu.segment.es) : /* <Empty> */ : "memory");
  __asm__  volatile("movl %%ds, %0" :  "=D" (ctx->cpu.segment.ds) : /* <Empty> */ : "memory");


  // Save flags and instruction pointer (fixed constraint)
  __asm__ volatile (
    "pushfl\n"
    "popl %0\n"
    "call 1f\n"
    "1: popl %1\n"
     : "=m" (ctx->eflags),
       "=m" (ctx->eip)
  );
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

__attribute__((noreturn)) void process_context_switch(struct Context ctx) {
    // Separate block for setting the page directory
    __asm__ volatile (
        "movl %0, %%cr3\n"             // Load page directory
        :
        : "r"(ctx.page_directory_virtual_addr)
        : "memory"
    );

    // Load CPU registers
    __asm__ volatile (
        "movl %0, %%ebx\n"
        "movl %1, %%edx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%eax\n"
        "movl %4, %%esi\n"
        "movl %5, %%edi\n"
        "movl %6, %%ebp\n"
        "movl %7, %%esp\n"
        :
        : "g"(ctx.cpu.general.ebx),
          "g"(ctx.cpu.general.edx),
          "g"(ctx.cpu.general.ecx),
          "g"(ctx.cpu.general.eax),
          "g"(ctx.cpu.index.esi),
          "g"(ctx.cpu.index.edi),
          "g"(ctx.cpu.stack.ebp),
          "g"(ctx.cpu.stack.esp)
    );

    // Load segment registers
    __asm__ volatile (
        "movl %0, %%gs\n"
        "movl %1, %%fs\n"
        "movl %2, %%es\n"
        "movl %3, %%ds\n"
        :
        : "g"(ctx.cpu.segment.gs),
          "g"(ctx.cpu.segment.fs),
          "g"(ctx.cpu.segment.es),
          "g"(ctx.cpu.segment.ds)
    );

    // Set up iret stack 
    __asm__ volatile (
        "pushl %0\n"                  // Push eflags
        "pushl %1\n"                  // Push eip
        "iret\n"
        :
        : "g"(ctx.eflags),
          "g"(ctx.eip)
    );

    __builtin_unreachable();
}


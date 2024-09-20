#include "pit.h"
#include "lib.h"


// references:
// https://wiki.osdev.org/Programmable_Interval_Timer
// http://www.osdever.net/bkerndev/Docs/pit.htm
/* 
 * init_pit
 *   DESCRIPTION: Initialize the PIT and set hardware freqency to default frequency
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: disable and enable NMI and setting the periodic interrupt enable bit 
 */
void init_pit()
{
    cli();
    enable_irq(PIT_IRQ);
    pit_change_freq(SCHEDULING_FREQ);
    sti();
}

/* pit_change_freq
 *   DESCRIPTION: Changes how often a PIT interrupt is generated. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_change_freq(uint32_t freq)
{
    uint32_t divisor = DEFAULT_PIT_FREQ / freq; /* Calculate our divisor */
    // NOTE: outb(data, port)
    outb(PIT_CMD, CMD_PORT);                         /* Set our command byte 0x36 */
    outb(divisor & LOWER_BYTE_MASK, DATA_PORT_CH_0); /* Set low byte of divisor */
    outb(divisor >> 8, DATA_PORT_CH_0);              /* Set high byte of divisor */
}

int32_t active_terminal = 0;        // The terminal number scheduled currently
int32_t done_init_terminal = 0;     // Flag for done initializing all three terminals
int32_t init_count = 0;         // Flag for initializing purposes

/* pit_intr
 *   DESCRIPTION: PIT interrpt handler. Sends in the eoi signal after interrupt is handled. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_intr()
{
    // cli();
    send_eoi(PIT_IRQ);      // Enables interrupt

    // Saves esp, ebp of the program right before entering scheduler
    uint32_t esp, ebp;
    asm volatile("\t movl %%esp,%0"
                 : "=r"(esp));
    asm volatile("\t movl %%ebp,%0"
                 : "=r"(ebp));
    
    // If pit fires before first terminal is initialized, do nothing
    if (terminal[0].init_flag == 0) {
        // sti();
        return;
    }

    // If terminal 0 is initialied, initialize terminal 1 
    if (terminal[0].init_flag == 1 && terminal[1].init_flag == 0 && terminal[2].init_flag == 0 && init_count == 0) {
        init_count = 1;
        
        // Save esp, ebp of the 0th terminal to its pcb
        pcb_t *current_pcb = (pcb_t *)get_pcb_blk(terminal[active_terminal].pid);
        current_pcb->active_ebp = ebp;
        current_pcb->active_esp = esp;
        
        active_terminal = (active_terminal + 1) % 3;

        // Map the user program page to the next shell to be initialized 
        remap_video_mem_user(active_terminal);
        unsigned char command[10] = "shell";
        execute_call(command);
        // sti();
        return;
    }

    // If terminal 0 and 1 are initialied, initialize terminal 2
    if (terminal[0].init_flag == 1 && terminal[1].init_flag == 1 && terminal[2].init_flag == 0 && init_count == 1) {
        init_count = 2;
        
        // Save esp, ebp of the 1st terminal to its pcb
        pcb_t *current_pcb = (pcb_t *)get_pcb_blk(terminal[active_terminal].pid);
        current_pcb->active_ebp = ebp;
        current_pcb->active_esp = esp;
        
        active_terminal = (active_terminal + 1) % 3;

        // Map the user program page to the next shell to be initialized 
        remap_video_mem_user(active_terminal);
        unsigned char command[10] = "shell";
        execute_call(command);
        // sti();
        return;
    }

    done_init_terminal = 1;

    // Save esp, ebp of the current process to its pcb
    pcb_t *current_pcb = (pcb_t *)get_pcb_blk(terminal[active_terminal].pid);
    current_pcb->active_ebp = ebp;
    current_pcb->active_esp = esp;
    // terminal[active_terminal].rtc_counter = rtc_counter;
    // terminal[active_terminal].rtc_target = rtc_target;

    active_terminal = (active_terminal + 1) % 3;
    
    // Update user video memory mapping and user program mapping
    remap_video_mem_user(active_terminal); 
    virt_to_phys_user(terminal[active_terminal].pid);
    
    // Continue from the esp, ebp of the next program saved before
    pcb_t *next_pcb = (pcb_t *)get_pcb_blk(terminal[active_terminal].pid);
    uint32_t next_esp = next_pcb->active_esp;
    uint32_t next_ebp = next_pcb->active_ebp;
    
    // Update tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = get_kernel_stack_addr(terminal[active_terminal].pid); // should be equal to (8kb*(cur_pid+1)- 4)
    
    asm volatile("\t movl %0, %%esp"
                 :
                 : "r"(next_esp));
    asm volatile("\t movl %0, %%ebp"
                 :
                 : "r"(next_ebp));
    asm volatile("         \n\
                leave   \n\
                     \n\
                ret     \n\
             ");
}

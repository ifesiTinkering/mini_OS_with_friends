#include "task.h"

/* get_kernel_stack_addr
 * Inputs:  process_id - pid of the current process  
 * Return Value: 0 if success else -1
 * Function: gets the address of task's stack in the kernel memory */
uint32_t get_kernel_stack_addr(uint32_t process_id){
   uint32_t addr = EIGHT_MB - ((process_id+1) * EIGHT_KB) - 4; //you may simply place the first task’s kernel stack at the bottom of the 4 MB kernel
   return addr;
}


/* get_pcb_addr
 * Inputs:  process_id - pid of the current process  
 * Return Value: 0 if success else -1
 * Function: gets the address of task's stack in the kernel memory */
uint32_t get_pcb_addr(uint32_t process_id){
  // uint32_t addr = get_kernel_stack_addr(process_id) - EIGHT_KB; //you may simply place the first task’s kernel stack at the bottom of the 4 MB kernel
   uint32_t addr = EIGHT_MB - (process_id+1)*EIGHT_KB;
   return addr;
}


/*init_process_id_array
 * Inputs:  NONE
 * Return Value: NONE
 * Function: initializes the pid array for all 6 processes. All pid are free */
void init_process_id_array(){
   int i;
   for( i = 3; i < MAX_PROCESSES; i++){
      pid_array[i] = 0;   //all process_id initialized to 0
   }
}


/*virt_to_phys_user
 * Inputs:  NONE
 * Return Value: NONE
 * Function: maps the 4 MB page for user programs at virtual addres at 128MB  */

void virt_to_phys_user(uint32_t curr_pid)
{   
   uint32_t process_id = curr_pid;
   //get the first 10 bits in virtual address for pafe directory address
   //physical address can either be 8 MB or 12 MB 
   uint32_t physical = (FOUR_MB * process_id) + EIGHT_MB; //each program has 4 MB allocated 
   uint32_t pdi = (VIRT_ADDR_PROGRAM & MASK_10_BITS) >> PD_INDEX_OFFSET; 
   page_directory[pdi] = ((uint32_t)physical & MASK_10_BITS) | P | R_W | U_S | PS; //points to the 4 MB page at physical addr 
  
   flush_tlb();
	return; 
}

/*get_pcb_blk
 * Inputs:  curr_process_id: pid of the pcb whose address needs to be found 
 * Return Value: address of the pcb block in the kernel 
 * Function: gets the address of the pcb block using the given pid  */
extern int32_t* get_pcb_blk(int32_t curr_process_id){
   // Returns address at the top of each corresponding kernel stack
   int32_t* ret = (int32_t*)(EIGHT_MB - (EIGHT_KB*(curr_process_id+1)));
   return ret;
}

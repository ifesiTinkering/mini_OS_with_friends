#include "paging.h"


// Using assembly to load CR3 with the address of the page directory 
void loadPageDirectory(unsigned int*);
// Using assembly to set the paging (PG) and protection (PE) bits of CR0.
void enablePaging();

/* init_paging
 *   DESCRIPTION: Initializing a page table and page directory for 0MB-8MB address
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Paging for first 3 checkpoints are done
 */
void init_paging(){
    int i;

    // First setting up the page table 
    for(i = 0; i < ENTRY_NUM; i++){
    // The memory address should start at 0x00 and increment by 4KB
    // The last 12 bits are not used for memory address but for flags, which should be turned on as below.
    // Set to supervisor level, read/write, not present
        page_table[i] = (i * FOUR_KB) | R_W;   
    }
 	
    // For the entry representing video memory, set to the corresponding memory address
    // and set to supervisor level, read/write, present.
    // VIDEO_INDEX must be (0XB8000/0X1000) = 0XB8
    page_table[VIDEO_INDEX] = ((VIDEO_INDEX * FOUR_KB) | P | R_W );
    page_table[VIDEO_INDEX + 1] = (((VIDEO_INDEX + 1) * FOUR_KB) | P | R_W); // 1st terminal screen
    page_table[VIDEO_INDEX + 2] = (((VIDEO_INDEX + 2) * FOUR_KB) | P | R_W); // 2nd terminal screen
    page_table[VIDEO_INDEX + 3] = (((VIDEO_INDEX + 3) * FOUR_KB) | P | R_W); // 3rd terminal screen

    // Setting first entry to have the address of page table
    // USE MASK TO ONLY GET THE FIRST 20 BITS FOR ADDRESS
    page_directory[0] = (uint32_t)( (uint32_t)page_table & MASK_20_BITS) | P | R_W;
    
    // Setting second entry to have the address of kernel
    // USE MASK TO ONLY GET FIRST 10 BITS FOR THIS 4MB PAGES 
    // SET PRESENT, R/W, SUPERVISOR AND PSE BIT FOR 4 MiB PAGES

    page_directory[1] = (KERNEL & MASK_10_BITS) | P | R_W | PS; 
    flush_tlb();
    // Setting every other unused 4MB chunks to not present
    for(i = 2; i < ENTRY_NUM; i++){
        page_directory[i] = EMPTY_PDE;
    }
    
    // Using assembly to load CR3 with the address of the page directory 
    loadPageDirectory(page_directory);
    
    // Using assembly to set the paging (PG) and protection (PE) bits of CR0.
    enablePaging();
}

void remap_video_mem_user(int32_t terminal_id){ 
  // terminal_id is active terminal
  // we need to point to the terminal video memory pages from here since pagetable[0] has DPL = 0 and no user level access so we cannot make changes to it
  // test_flag = 44;
  // user directory at  132 MB. 0- 4MB directory is not user level
  // test_flag = 11;
  page_directory[33] = (uint32_t)(uint32_t)page_table_video | U_S | R_W | P;

  if (displayed_terminal == terminal_id)
    page_table_video[0] = ((uint32_t)VIDEO & MASK_20_BITS) | U_S | R_W | P;

  else
    page_table_video[0] = ((uint32_t)(VIDEO + ((terminal_id + 1) * FOUR_KB)) & MASK_20_BITS) | U_S | R_W | P;

  // flush TLB
  flush_tlb();
  // test_flag = 4444;
  return;
}


/* flush_tlb()
 * Inputs:  None
 * Return Value: None
 * Function: flushes TLB
*/
//Referenced OSDEV TLB: https://wiki.osdev.org/TLB
void flush_tlb(void){
	asm volatile("                    \n\
                  movl %%cr3, %%eax   \n\
                  movl %%eax, %%cr3   \n\
                 " 
                 :                      
                 :                      
                 :"%eax"     /*clobbered*/     
                 );
}
/*should be eqivalent to:

    movl %cr3, %eax    
    movl %eax, %cr3
    retun to kernel (not iret) 

    source: TA 
    not sure if this returns or if we are swapping orders
*/


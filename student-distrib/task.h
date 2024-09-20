#ifndef _TASK_H
#define _TASK_H

#include "system_calls.h"
#include "types.h"
#include "paging.h"
#include "x86_desc.h"


#define MAX_PROCESSES           6
#define ENTRY_POINT_FILE_POS    24
#define ENTRY_MAGIC_LENGTH      4
#define VIRT_VIDEO_PAGE         EIGHT_MB * (MAX_PROCESSES+1)


extern int32_t* get_pcb_blk(int32_t curr_process_id);
extern void init_process_id_array();
extern uint32_t get_kernel_stack_addr(uint32_t process_id);
uint32_t get_pcb_addr(uint32_t process_id);
extern int32_t get_valid_process_id();

extern void virt_to_phys_user(uint32_t curr_pid);
#endif

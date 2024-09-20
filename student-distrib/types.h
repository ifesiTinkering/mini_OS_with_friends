/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0
#define TERMINAL_NUM 3
#define MAX_PROC_TER 4

#define BUFMAX          128
#define FILES_OPEN_MAX  8

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

typedef struct terminal_sc{
   unsigned char tempbuf[128];         
   char* videoMem;
   int32_t terminal_id;
   int32_t length;
   int32_t read_length;
   int32_t x;
   int32_t y;
   int32_t pid;
   int32_t parentpid;
   int32_t halt_flag;
   volatile int32_t shift_up_one_flag;
   volatile int32_t init_flag;
   volatile int32_t enter_flag;
   volatile int32_t rtc_flag;
   volatile uint32_t rtc_counter;
   volatile uint32_t rtc_target;
   uint32_t only_used_for_zero;
} terminal_t;

// uint8_t pid_history[TERMINAL_NUM][MAX_PROC_TER];
int test_flag ;

extern int32_t pid_array[6]; //0 if the process_id is not used.
extern int32_t curr_pid;

extern terminal_t terminal[3];
extern int32_t displayed_terminal; 
extern int32_t active_terminal;
extern int32_t done_init_terminal;

int shared_btw_terminal_fs_drivers;
int large_text_blk_count;
int putc_flag;

/* file operations table to unify all cp 2 operations (gotten fom foamnt of all system calls) */
typedef struct file_operation_table{
    int32_t (*open_call_entry)(const void* fname);
    int32_t (*close_call_entry)(int32_t fd);
    int32_t (*read_call_entry)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write_call_entry)(int32_t fd, const void* buf, int32_t nbytes);  
} op_table_t;


/* file descriptor*/
typedef struct file_desc_table{
    op_table_t* table_ptr; //file operations atable ptr
    uint32_t inode_num; //idnentufying innode number
    uint32_t file_position; //current postion in opned file
    uint32_t flags; //occupied or free
} file_desct_table_t;


/*per-task process control block*/
typedef struct process_control_blk{
    file_desct_table_t file_desc_array[FILES_OPEN_MAX]; //file desciptor array
    uint32_t active;        //tells if pcb is active or not
    uint32_t process_id;    //process id to identify process (goes from 0-5 inclusive)
    uint32_t parent_process_id; //process id of preceeding process
    uint32_t task_esp;      //esp for task
    uint32_t task_ebp;      //ebp for task
    uint32_t active_esp;      //esp for task
    uint32_t active_ebp;      //ebp for task


    uint8_t terminal_cmd[BUFMAX];// accompaning command for task
    int8_t terminal_on;
    /*following may not be used in current implemtenaation*/
    uint32_t kernel_stack_addr;
    uint32_t program_stack_addr;    // 12MB-1, 16MB-1, etc
    uint32_t program_page_addr;     // 8MB, 12MB, etc
    uint32_t program_image_addr;    // 8MB+OFFSET, 12MB+OFFSET, etc
} pcb_t;

/*operation tables generated fro different file types. these get set in kernel*/
op_table_t files_operations;
op_table_t directories_operations;
op_table_t stdin_operations;
op_table_t stdout_operations;      
op_table_t rtc_operations;
op_table_t terminal_operations;
op_table_t nothing_operations;

#endif /* ASM */
#endif /* _TYPES_H */

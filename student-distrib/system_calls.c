#include "system_calls.h"

int32_t curr_pid = SHELL_PID;
int cur_indx;
int save_parent_pid;

// Array that indicates whether a pid is valid
// First three are designed to be one shell in each terminal
int32_t pid_array[MAX_PROCESSES]= {1, 1, 1, 0, 0, 0};   

// This function serves as a place holder
int32_t ignoreThisEntry(void){
    printf("ignoreThisEntry happening rn");
    return 0;
}


int32_t get_curr_pid(void){
    return curr_pid;
}


/* halt_call
 *   DESCRIPTION: terminates a process, returning the specified value to its parent process.
 *   INPUTS: status -- argument to be extended into the 32-bit return value to parent execute call
 *   OUTPUTS: none
 *   RETURN VALUE: 0 at all times
 *   SIDE EFFECTS: This call should never return to the caller
 */
int32_t halt_call(uint8_t status){
    // Update curr_pid to the lastest pid in current displayed terminal
    curr_pid = terminal[active_terminal].pid;

    // Reset rtc values to shell
    terminal[active_terminal].rtc_counter = DEFAULT_COUNTER;
    terminal[active_terminal].rtc_target = DEFAULT_TARGET;

    // If progrma to halt is base shell of a terminal
    if (curr_pid < 3){
        // UNMAP SHELL 
        // RESET GLOBAL STATE TRACKING VALUES
        curr_pid = SHELL_PID + active_terminal; //curr_pid should be 0,1,2 for the three terminals 
        terminal[active_terminal].pid = curr_pid;
        save_parent_pid = ERROR;
        terminal[active_terminal].parentpid = save_parent_pid;
        terminal[active_terminal].init_flag = 0;
        // terminal[active_terminal].halt_flag = 1; 
        // call shell again
        unsigned char command[10] = "shell";
        execute_call(command);
        return 0; 
    }
    
    // for every other halt that is not the first shell
    // BOOKKEEPING: getting parent pcb and current process pcb
    if(displayed_terminal == active_terminal)
       clear_temp_buff();
    //terminal[active_terminal].halt_flag = 1;   
    pcb_t *curr_pcb = (pcb_t *)get_pcb_addr((int32_t)curr_pid);
    pcb_t *parent_pcb = (pcb_t *)get_pcb_addr((int32_t)terminal[active_terminal].parentpid);
    // closing current process' file desciptors
    int idx;
    for (idx = 0; idx < FILES_OPEN_MAX; idx++){
        curr_pcb->active = NOT_ENGAGED;
        curr_pcb->terminal_on = -1;
        curr_pcb->file_desc_array[idx].flags = NOT_ENGAGED;
    }

    //reset global state tracking varibales. freeing current process pid and switching to parent process data 
    curr_pid = (int32_t)terminal[active_terminal].parentpid;     // Update curr_pid to parent pid
    terminal[active_terminal].pid = curr_pid;                    // Reflect above change in terminal struct
    save_parent_pid = parent_pcb->parent_process_id;                
    terminal[active_terminal].parentpid = save_parent_pid;
    pid_array[curr_pcb->process_id] = FREE;                    // Free the unused pid

    virt_to_phys_user(curr_pid); //get the parent process page
 

    //set tss data
    tss.ss0 = KERNEL_DS;
    tss.esp0 = get_kernel_stack_addr(curr_pid); 

    //take in esp, ebp and return value
    //set esp, ebp register as esp and ebp argumnets 
    //set eax register as the return value
    //eax - return value
    asm volatile("              \n\
                mov %0, %%eax   \n\
                mov %1, %%esp   \n\
                mov %2, %%ebp   \n\
                jmp HALT_FLAG   \n\
                "
                 :
                 : "r"((uint32_t)status), "r"(curr_pcb->task_esp), "r"(curr_pcb->task_ebp)
                 : "%eax"     /*clobbered*/  
                );
                 
    return 0;
}


/* execute_call
 *   DESCRIPTION: load and execute a new program, 
 *                handing off the processor to the new program until it terminates
 *   INPUTS: command -- space-separated sequence of words containing file name and args
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if the command cannot be executed,
 *                 256 if the program dies by an exception
 *                 0 to 255 if the program executes a halt system call which the halt provides return value
 *   SIDE EFFECTS: none
 */
int32_t execute_call(const uint8_t *command)
{
    cli();
    if(displayed_terminal == active_terminal)
       clear_temp_buff();
    // terminal[active_terminal].halt_flag = 0;   
    curr_pid = terminal[active_terminal].pid;
    uint32_t user_stack;
    // check if command exceed the 1024
    if (1023 < strlen((char *)command))
        return -1;

    // retrieve the first word, clear the space before executable
    int index = 0;
    while (*command == ' '){
        command++;
    }
    
    const uint8_t *scan;
    uint8_t cmd[MAXBUFSIZE]; // stores the first word
    int i;
    for (i = 0; i < MAXBUFSIZE; i++) {
        cmd[i] = 0;
    }

    for (scan = command; *scan != ' ' && *scan != '\0' && *scan != '\n'; scan++)
    {
        cmd[index++] = *scan;
    }
   
    // check if the command contains file name
    int valid;
    dentry_t dentry_input;
    dentry_t *dentry_ptr;
    dentry_ptr = &dentry_input;

    // check if the executable is a valid file
    valid = read_dentry_by_name(cmd, dentry_ptr);
    if (valid != 0){
        return -1;
    }

    // retrieve the firt 4 bytes inside the file
    int8_t inode = dentry_ptr->inode_num;
    uint32_t offset = 0;
    uint8_t buf[MAXBUFSIZE];
    valid = read_data(inode, offset, buf, 4);
    
    if (valid == -1)
        return -1;

    // check if the first 4 byte is : 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46.
    if (buf[0] != 0x7F || buf[1] != 0x45 || buf[2] != 0x4C || buf[3] != 0x46)
        return -1;

    // store the second arg into pcb
    const uint8_t *scan2;
    uint8_t arg[MAXBUFSIZE]; // stores the second arguments
    for (i = 0; i < MAXBUFSIZE; i++) {
        arg[i] = 0;
    }
    while (*scan == ' '){
        scan++;
    }
    index = 0;
    for (scan2 = scan; *scan2 != '\0' && *scan2 != '\n'; scan2++) {
        arg[index++] = *scan2;
    }

    // Save the pid before executing
    int32_t old_curr_pid;
    old_curr_pid = curr_pid;
    
    // If we are executing base shell
    if (terminal[active_terminal].init_flag == 0) {
        curr_pid = terminal[active_terminal].pid;
        terminal[active_terminal].parentpid = ERROR;
        terminal[active_terminal].init_flag = 1;
    } else {
        for (i = 3; i < 6; i++){ 
            // find and record curr pid
            // can have max of 6 processes
            if (pid_array[i] == 0){
                pid_array[i] = 1; //process_id now used
                curr_pid = i;
                break;
            }
        }
        // no process_id free
        if (i >= 6)
            return -1; 
    }
    
    // Update pid in terminal struct
    if (terminal[active_terminal].init_flag == 1) {
    terminal[active_terminal].pid = curr_pid;
    }
    // copy file into page
    inode_t *curr_inode_ptr = (inode_t *)(inode_addr + inode);
    int32_t curr_length = curr_inode_ptr->length;
    
    // setting up paging: for every task
    virt_to_phys_user((int32_t)curr_pid);                                                       // setting up a 4 MB page for a task.
    pcb_t *curr_pcb = (pcb_t *)get_pcb_addr((int32_t)curr_pid); 
   
    // givin in doc as prograM img addrress 0x08048000
    valid = read_data(inode, 0, (uint8_t *)PROGRAM_ADDR, curr_length); 
    if (valid == -1)
        return -1;
    
    //get store curr pid and parent pid 
    int shell_flag = 0;
    if (curr_pid < 3)
        shell_flag = 1;

    // Determine whether curr process is shell and save parent pid accordingly
    if (shell_flag) {
        save_parent_pid = active_terminal;
        terminal[active_terminal].parentpid = ERROR;
        curr_pcb->parent_process_id = terminal[active_terminal].parentpid;

    }
    else {
        save_parent_pid = old_curr_pid;
        terminal[active_terminal].parentpid = save_parent_pid;
        curr_pcb->parent_process_id = save_parent_pid;

    }
    curr_pcb->process_id = curr_pid;
    curr_pcb->active = 1; //ake process active 
    curr_pcb->terminal_on = active_terminal;
    
    // Setting up a file descriptor
    // The first two entries are stdin and stdout
    op_table_t *termin = &stdin_operations;
    op_table_t *termOUT = &stdout_operations;
    curr_pcb->file_desc_array[0].table_ptr = termin;
    curr_pcb->file_desc_array[0].inode_num = NOINODE;
    curr_pcb->file_desc_array[0].file_position = START_OF_FILE;
    curr_pcb->file_desc_array[0].flags = ENGAGED;

    curr_pcb->file_desc_array[1].table_ptr = termOUT;
    curr_pcb->file_desc_array[1].inode_num = NOINODE;
    curr_pcb->file_desc_array[1].file_position = START_OF_FILE;
    curr_pcb->file_desc_array[1].flags = ENGAGED;
    
    // Set the rest as empty place holders
    int idx;
    for (idx = 2; idx < 8; idx++)
    {
        op_table_t *nothing_operations_ptr = &nothing_operations;
        curr_pcb->file_desc_array[idx].table_ptr = nothing_operations_ptr;
        curr_pcb->file_desc_array[idx].inode_num = NOINODE;
        curr_pcb->file_desc_array[idx].file_position = START_OF_FILE;
        curr_pcb->file_desc_array[idx].flags = NOT_ENGAGED;
    }
    // need command WHEN IMPLEMNTING GET ARGS
    int curr_cmd_index = 0;
    uint8_t curr_cmd_char = arg[curr_cmd_index];
    while(curr_cmd_char){
        curr_pcb->terminal_cmd[curr_cmd_index] = curr_cmd_char;
        curr_cmd_index++;
        curr_cmd_char = arg[curr_cmd_index];
    }
    curr_pcb->terminal_cmd[curr_cmd_index] = 0;
   
   // Read the entry point
    uint8_t entry_addr[4];
    read_data(inode, ENTRY_POINT_FILE_POS, entry_addr, ENTRY_MAGIC_LENGTH);
    uint32_t entry_point = (entry_addr[3] << 24) + (entry_addr[2] << 16) + (entry_addr[1] << 8) + entry_addr[0]; // little endian

    // the DS register must be set to point to the correct entry in the GDT for the user mode data segment (USER DS) before you execute the IRET instruction
    // user stack is the bottom of each user page (esp)
    // kernel stack is the bottom of kernel page
    // Setup tss: Appemdix E
    // stack segment: points to the kernel’s stack segment
    tss.ss0 = KERNEL_DS;
    // stack pointer: points to process’s kernel-mode stack
    tss.esp0 = get_kernel_stack_addr(curr_pid); // should be equal to (8kb*(cur_pid+1)- 4)

    // Saving current eip and esp to pcb before saving things on stack
    int32_t esp, ebp;
    // https://stackoverflow.com/questions/2114163/reading-a-register-value-into-a-c-variable
    asm volatile("\t movl %%esp,%0"
                 : "=r"(esp));
    asm volatile("\t movl %%ebp,%0"
                 : "=r"(ebp));
    curr_pcb->task_esp = esp;
    curr_pcb->task_ebp = ebp;
   
    // ready for privilege switching
    // sti();                           // on eTA told me pushfl below include sti

    // order is ss, esp, eflags, cs && eip
    // (https://unix.stackexchange.com/questions/510960/why-does-linux-have-two-data-segments-one-for-user-mode-and-another-for-kernel)
    // Linux uses the same segment descriptor for SS (the stack segment) and DS     (the data segment).
    // SS: point to the correct entries in the Global Descriptor Table that correspond to and user-mode stack segments  (USER_CS)
    // ESP:  the bottom of the 4 MB page already holding the executable image.      (user_stack)
    // EFLAGS
    // CS: point to the correct entries in the Global Descriptor Table that correspond to the user-mode code    (USER_CS)
    // EIP:  jump to is the entry point from bytes 24-27 of the executable that you have just loaded    (entry_point)
    // unsigned cs_value = USER_CS || 0x03;

    user_stack = VIRT_ADDR_PROGRAM + FOUR_MB - 4;
    
    asm volatile("      \n\
            push   %0   \n\
            pushl  %1   \n\
            pushfl      \n\
            push   %2   \n\
            push   %3   \n\
            iret        \n\
            HALT_FLAG:  \n\
            leave       \n\
            ret         \n\
            "
            :
            : "r"(USER_DS), "r"(user_stack), "r"(USER_CS), "r"(entry_point)
            );
    return 0;
}


/* write_call
 *   DESCRIPTION: writes data to the terminal or to a device (RTC)
 *   INPUTS: fd -- input file descriptor
 *           buf -- the input buffer to write
 *           nbytes -- the length of input to write
 *   OUTPUTS: none
 *   RETURN VALUE: return -1 to indicate failure since the file system is read-only
 *                 returns the number of bytes written
 *   SIDE EFFECTS: none
 */
int32_t write_call(int32_t fd, const void *buf, int32_t nbytes)
{
    // Checking if fd is valid
    if (fd < 0 || fd >= 8 || buf == NULL || nbytes <= 0)
        return ERROR;

    // Checking if flag is engaged
    pcb_t* cur_pcb_addr = (pcb_t*) get_pcb_blk(terminal[active_terminal].pid);
    int checkflag = cur_pcb_addr->file_desc_array[fd].flags;
    if (checkflag == NOT_ENGAGED)
        return ERROR;
    
    // Call top level write function
    int ret = cur_pcb_addr->file_desc_array[fd].table_ptr->write_call_entry(fd, buf, nbytes);
    if (buf == NULL)
        return ERROR;
    else
        return ret;
    
}

/* write_call
 *   DESCRIPTION: provides access to the file system
 *   INPUTS: filename -- target file name
 *   OUTPUTS: none
 *   RETURN VALUE: return -1 if the named file does not exist or no descriptors are free
 *                 returns the allocated file descriptor
 *   SIDE EFFECTS: none
 */
int32_t open_call(const uint8_t *filename){
    // Find the corresponding dentry
    dentry_t dentry;
    dentry_t *dentry_ptr = &dentry;
   // printf("file name is: '%s' \n", filename);
    if ((filename == NULL) ||(ERROR == read_dentry_by_name((uint8_t *)filename, dentry_ptr)))
        return ERROR;

    // Setting up placeholders
    op_table_t *PTR_files_operations = &files_operations;
    op_table_t *PTR_directories_operations = &directories_operations;
    op_table_t *PTR_rtc_operations = &rtc_operations;
    op_table_t *PTR_nothing_operations = &nothing_operations;

    pcb_t* cur_pcb_addr = (pcb_t*) get_pcb_blk(terminal[active_terminal].pid);

    int idx;
    int found_flag = 0;
    for (idx = 2; idx <= 7; idx++)
    {
        if (found_flag == 1)   
            break;    

        // Checks whether current idx can be used
        if (cur_pcb_addr->file_desc_array[idx].flags == NOT_ENGAGED){
            found_flag = 1;

            // Associate current fd entry to the inode number
            cur_pcb_addr->file_desc_array[idx].file_position = START_OF_FILE;
            cur_pcb_addr->file_desc_array[idx].flags = ENGAGED;
            cur_pcb_addr->file_desc_array[idx].inode_num = dentry.inode_num;
                
            // Check the file type and associate corresponding operations
            switch (dentry_ptr->filetype){
            case 0:
                // file type is rtc
                cur_pcb_addr->file_desc_array[idx].table_ptr = PTR_rtc_operations;
                break;

            case 1:
                // file type is directory
                cur_pcb_addr->file_desc_array[idx].table_ptr = PTR_directories_operations;
                break;

            case 2:
                // file type is other than terminal
                cur_pcb_addr->file_desc_array[idx].table_ptr = PTR_files_operations;
                break;

            default:
                // Assign no operations
                cur_pcb_addr->file_desc_array[idx].table_ptr = PTR_nothing_operations;
                break;
            }
        }
    }

    if (found_flag == 0)
        return ERROR;

    return idx-1;   
}


/* close_call
 *   DESCRIPTION: closes the specified file descriptor and makes it available 
 *   INPUTS: fd -- target file desciptor
 *   OUTPUTS: none
 *   RETURN VALUE: return -1 if closing an invalid descriptor, return 0 upon successful close
 *   SIDE EFFECTS: none
 */
int32_t close_call(int32_t fd){

    // since we can't close stdin and stdout, so checking if fd is within 2-7
    if (fd <= 1)
        return ERROR;
    
    if (fd>=8)
        return ERROR;
    
    pcb_t* curr_pcb_addr = (pcb_t*) get_pcb_blk(terminal[active_terminal].pid);

    int checkflag =curr_pcb_addr->file_desc_array[fd].flags;
    if (checkflag == NOT_ENGAGED)
        return -1;

    curr_pcb_addr->file_desc_array[fd].inode_num = NOINODE;
    curr_pcb_addr->file_desc_array[fd].file_position = START_OF_FILE;
    curr_pcb_addr->file_desc_array[fd].flags = NOT_ENGAGED;

    int32_t ret = curr_pcb_addr->file_desc_array[fd].table_ptr->close_call_entry(fd);
    return ret;
}




/* getargs_call
 *   DESCRIPTION: Reads the program’s command line arguments into a user-level buffer
 *   INPUTS: buf -- the buffer (in user space) that will be occupied
 *           nbytes -- buffer length
 *   OUTPUTS: none
 *   RETURN VALUE:  -1 if no arguments, or buffer not large enough, or 0 if copy successful
 *   SIDE EFFECTS: The input buffer is first cleared
 */
int32_t getargs_call(uint8_t *buf, int32_t nbytes)
{
    // Checking valid buffer
    if (buf == 0)
        return ERROR;
    
    int write_length = nbytes;
    int i;
    
    // Clearing buffer
    for (i = 0; i < nbytes; i++) {
        buf[i] = 0;
    }
    
    int empty, curr_index;
    uint8_t curr_char;
    
    // Getting input command during execute
    pcb_t *temp_pcb = (pcb_t *)get_pcb_addr((uint32_t)terminal[active_terminal].pid);
    uint8_t *temp_cmd = temp_pcb->terminal_cmd;

    empty = 0;
    curr_index = 0;
    curr_char = temp_cmd[curr_index];
    
    // Write command to buffer until NULL or input length is reached
    while (curr_char != 0 && write_length)
    {
        buf[curr_index] = temp_cmd[curr_index];
        empty++;
        curr_index++;
        write_length--;
        curr_char = temp_cmd[curr_index];
    }
    
    // When buffer is empty
    if (empty == 0)
        return ERROR;
    
    return SUCCESS;
}


 
 /* vidmap_call
 *   DESCRIPTION: Maps the text-mode video memory into user space at a pre-set virtual address
 *   INPUTS: screen_start -- pointer to the screen_start pointer
 *   OUTPUTS: none
 *   RETURN VALUE:  -1 if invalid input, or 0 if successful
 *   SIDE EFFECTS: *screen_start now is the pointer to the user-accessible vid mem
 */
int32_t vidmap_call(uint8_t **screen_start)
{
    // Checking whether input is null pointer
    if (screen_start == 0)
        return ERROR;
    
    // check whether the address falls within the address range covered by the single user-level page
    if (((uint32_t)screen_start) < VIRT_ADDR_PROGRAM || ((uint32_t)screen_start) >= (VIRT_ADDR_PROGRAM + FOUR_MB))
        return ERROR;

    // page_table_video[VIDMAP_TABLE_INDEX] = ((VIDEO_INDEX * FOUR_KB) | P | R_W | U_S);
    pcb_t *curr_pcb = (pcb_t *)get_pcb_addr((int32_t) terminal[active_terminal].pid); 

    if(active_terminal == curr_pcb->terminal_on){
    // The virtual memory is chosen arbitrarily the first entry in new page table for convenience
    // and set to user level, read/write, present.
        page_table_video[VIDMAP_TABLE_INDEX] = ((VIDEO_INDEX * FOUR_KB) | P | R_W | U_S);
    } else {
        page_table_video[VIDMAP_TABLE_INDEX] = (((VIDEO_INDEX + curr_pcb->terminal_on + 1) * FOUR_KB) | P | R_W | U_S);
    }

    // Use the page that is immediately after user memory to be the page that contains virtual video memory
    page_directory[VIDMAP_PAGE_INDEX] = (uint32_t)((uint32_t)page_table_video & MASK_20_BITS) | P | R_W | U_S;
    
    flush_tlb();
    
    // screen_start should now be a pointer to the virtual video memory
    *screen_start = (uint8_t*) VIDMAP_ADDR;
    return SUCCESS;
}


/* set_handler_call
 *   DESCRIPTION: related to signal handling, not supported by OS
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: return failure from these calls
 *   SIDE EFFECTS: none
 */
int32_t set_handler_call(int32_t signum, void *handler_address)
{
    printf("set_handler_call happening rn");
    return 0;
}

/* sigreturn_call
 *   DESCRIPTION: related to signal handling, not supported by OS
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: return failure from these calls
 *   SIDE EFFECTS: none
 */
int32_t sigreturn_call(void)
{

    printf("sigreturn_call happening rn");
    return 0;
}


/* read_call
 *   DESCRIPTION: reads data from the keyboard, a file, device (RTC), or directory
 *   INPUTS: fd -- input file descriptor
 *           buf -- the input buffer to write
 *           nbytes -- the length of input to write
 *   OUTPUTS: none
 *   RETURN VALUE: return -1 if invalid input address
 *   SIDE EFFECTS: none
 */
int32_t read_call(int32_t fd, void *buf, int32_t nbytes)
{

    // Checking invalid fd
    if (fd < 0 || fd >= 8 || buf == NULL || nbytes <= 0)
        return ERROR;

    // Checking whether flag is engaged
    pcb_t* cur_pcb_addr = (pcb_t*) get_pcb_blk(terminal[active_terminal].pid);
    int checkflag = cur_pcb_addr->file_desc_array[fd].flags;
    if (checkflag == NOT_ENGAGED)
        return ERROR;

    // Use jump table to call corresponding read functions
    int ret = cur_pcb_addr->file_desc_array[fd].table_ptr->read_call_entry(fd, buf, nbytes);

    // Check invalid buffer
    if (buf == NULL)
        return ERROR;
    
    // Increment file position by the number of bytes read for successful future reads 
    cur_pcb_addr->file_desc_array[fd].file_position= cur_pcb_addr->file_desc_array[fd].file_position+ret;

    return ret; 
}

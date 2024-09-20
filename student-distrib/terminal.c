#include "terminal.h"

int32_t displayed_terminal = 0; // global variable to keep track of which terminal is shown on screen

int32_t donothing()
{
    printf("terminal do nothing");
    return -1;
};

/* int32_t terminal_read(int32_t fd, int8_t *buf, int32_t nbytes)
 * Inputs: fd: pointer to the terminal device driver file
 *             buf: holds the string to be printed on the video memory
 *             nbytes: no of bytes to be written on the terminal 
 * Return Value: no of bytes in one line 
 * Function:reads the data from one line after ener is pressed and stores it in buf */
int32_t terminal_read(int32_t fd, void *buf_terminal, int32_t nbytes)
{
    cli();
    int i, ret;
    i = 0;
    int8_t *buf = (int8_t *)buf_terminal;
    if (buf == NULL) // invalid pointer to the buffer
        return -1;

    terminal[active_terminal].enter_flag = 0;
    sti(); //enable interrupts

    // Allow interrupt while waiting for the enter flag
    while (get_flag() == 0);

    cli(); //disable interrupts
    // check to ensure we do not print more than 128 characters on the screen
    int length = get_len();
    if (MAX_BUF_SIZE <= length) {
        length = MAX_BUF_SIZE;
    }
    // storing the line in buffer
    for (i = 0; i < length; i++) {
        buf[i] = terminal[active_terminal].tempbuf[i];
    }
    buf_terminal = buf;
    // The return value should be the length copied
    ret = length;
    // Reset displayed terminal's length to 0
    // terminal[active_terminal].length = 0;
    sti();

    //returning the number of bytes in one line
    return ret;
}

/* int32_t terminal_write(int32_t fd, const int8_t *buf, int32_t nbytes)
 * Inputs: fd: pointer to the terminal device driver file
 *             buf: holds the string to be printed on the video memory
 *             nbytes: no of bytes to be written on the terminal 
 * Return Value: 0 for succesfully writing otherwise -1
 * Function:writes the string stored in buf on the terminal screen */
int32_t terminal_write(int32_t fd, const void *buf_terminal, int32_t nbytes)
{
    int i = 0;
    int8_t *buf = (int8_t *)buf_terminal;
    int curr_pib_index = active_terminal;
    if (buf == NULL || nbytes <= 0) // checking all the edges
        return -1;
    int length = nbytes;

    if (done_init_terminal == 0) {
        curr_pib_index = active_terminal;
    }
    else {
        pcb_t *current_pcb = (pcb_t *)get_pcb_blk(terminal[active_terminal].pid);
        curr_pib_index = current_pcb->terminal_on;
    }

     // prinitng the data onto the video memory
    for (i = 0; i < length; i++) {
        terminal_putc(buf[i], curr_pib_index);
    }

    buf_terminal = buf;
    terminal[active_terminal].length = 0;
    return 0; // successful call
}


/* int32_t terminal_open(const uint8_t *filename)
 * Inputs: filename: pointer to the terminal device driver file
 * Return Value: 0 for succesfully opening the device driver 
 * Function: opening the device driver file and clearing the video memory  */
int32_t terminal_open(const void *filename) {
    clear();
    return 0;
}

/* int32_t terminal_open(const uint8_t *filename)
 * Inputs: fd: pointer to the terminal device driver file
 * Return Value: 0 for succesfully closing the device driver 
 * Function:closing the device driver */
int32_t terminal_close(int32_t fd) {
    return 0;
}

/* init_terminals
 *   DESCRIPTION: Initialize terminal structs upon first entry
 *   INPUTS: i - the index of terminal we are initializing
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: All three terminals structs will be initialized
 */
void init_terminals(int32_t i)
{
    int j;
    for (j = 0; j < 3; j++)
    {
        terminal[j].terminal_id = j;
        // base pid for each terminal
        // terminal 0 has pid 0
        terminal[j].pid = j;

        // TA steven said that there are multiple implementation about bootup and execute shell in 3 terminals
        // I would just execute shell for each terminal when we initialize terminals
        terminal[j].x = 0;
        terminal[j].y = 0;
        // Stores the memory address of beginning of video memory of that terminal
        terminal[j].videoMem = (char *)((VIDEO_INDEX + j + 1) * FOUR_KB);
        terminal[j].enter_flag = 0;
        terminal[j].rtc_counter = DEFAULT_COUNTER;
        terminal[j].rtc_target = DEFAULT_TARGET;
        terminal[j].rtc_flag = 0;
        terminal[j].shift_up_one_flag = 0;
        terminal[j].halt_flag = 1; 
    }

    // Remaps the user video memory to 0th terminal
    remap_video_mem_user(0);
   
    //shell for terminal 0;
    unsigned char command[10] = "shell";
    execute_call(command);
    
}

/* save
 *   DESCRIPTION: Save the currently displayed terminal information
 *   INPUTS: terminal_id - the index of terminal we are saving
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Corresponding terminal struct will be modified
 */
void save(int32_t terminal_id) {
    // Save cursor location for each terminal
    terminal[terminal_id].x = get_screen_x();
    terminal[terminal_id].y = get_screen_y();
        
    // Update currently displayed info
    memcpy((char *)terminal[terminal_id].videoMem, (char *)VIDEO, FOUR_KB);
}

/* restore
 *   DESCRIPTION: Load the saved terminal information
 *   INPUTS: terminal_id - the index of terminal we are loading from
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Corresponding terminal struct will be modified
 */
void restore(int32_t terminal_id) {
    // Update currently displayed info
    memcpy((char *)VIDEO, (char *)terminal[displayed_terminal].videoMem, FOUR_KB);
}

/* open_terminal
 *   DESCRIPTION: Switch to a terminal upon keyboard entry
 *   INPUTS: terminal_id - the index of terminal we are switching to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int open_terminal(int32_t terminal_id)
{
    cli();

    // Return error if terminal id out of bound
    if (terminal_id > 2)
        return -1;

    // No need to switch when displayed is same as target terminal
    if (terminal_id == displayed_terminal)
        return 0;

    save(displayed_terminal);           // Save information about current terminal
    displayed_terminal = terminal_id;   // Update displayed terminal to the target terminal
    restore(displayed_terminal);
    
    remap_video_mem_user(displayed_terminal);   // Update user video memory mapping
    
    // Execute shell if terminal is not initialized
    if (terminal[displayed_terminal].init_flag == 0) {
        sti();        
        unsigned char command[10] = "shell";
        execute_call(command);
        return 0;
    }
    
    update_screen_xy(terminal[displayed_terminal].x, terminal[displayed_terminal].y);
    sti();
    return 0;
}

#include "keyboard.h"


volatile unsigned int shift, control, alt;
/* keyboard_init
 * Inputs:  None
 * Return Value: None
 * Function: initialize the keybaord device, connect keyboard to PIC */
void keyboard_init(void)
{

    enable_irq(keyboard_irq); // enabling irq 1 in pic.
    caps = 0;                 // initialize the capslock var
    shift = 0;
    control = 0;
    str_len = 0;
    alt = 0;
}

/* the global variable used to access alphanumerical values from the keyboard Scan code set 1
 * no shift&no caps; shift&no caps; no shift&caps; shift&caps
 */
static char keyboard_char[TOTAL_SCANCODES][SHADOW] = {
    {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {'1', '!', '1', '!'},
    {'2', '@', '2', '@'},
    {'3', '#', '3', '#'},
    {'4', '$', '4', '$'},
    {'5', '%', '5', '%'},
    {'6', '^', '6', '^'},
    {'7', '&', '7', '&'},
    {'8', '*', '8', '*'},
    {'9', '(', '9', '('},
    {'0', ')', '0', ')'},
    {'-', '_', '-', '_'},
    {'=', '+', '=', '+'},
    {BKSP, BKSP, BKSP, BKSP},
    {' ', ' ', ' ', ' '},
    {'q', 'Q', 'Q', 'q'},
    {'w', 'W', 'W', 'w'},
    {'e', 'E', 'E', 'e'},
    {'r', 'R', 'R', 'r'},
    {'t', 'T', 'T', 't'},
    {'y', 'Y', 'Y', 'y'},
    {'u', 'U', 'U', 'u'},
    {'i', 'I', 'I', 'i'},
    {'o', 'O', 'O', 'o'},
    {'p', 'P', 'P', 'p'},
    {'[', '{', '[', '{'},
    {']', '}', ']', '}'},
    {ENT, ENT, ENT, ENT},
    {0x00, 0x00, 0x00, 0x00}, // left control
    {'a', 'A', 'A', 'a'},
    {'s', 'S', 'S', 's'},
    {'d', 'D', 'D', 'd'},
    {'f', 'F', 'F', 'f'},
    {'g', 'G', 'G', 'g'},
    {'h', 'H', 'H', 'h'},
    {'j', 'J', 'J', 'j'},
    {'k', 'K', 'K', 'k'},
    {'l', 'L', 'L', 'l'},
    {';', ':', ';', ':'},
    {'\'', '\"', '\'', '\"'},
    {'`', '~', '`', '~'},
    {0x00, 0x00, 0x00, 0x00}, // left shift
    {'\\', '|', '\\', '|'},
    {'z', 'Z', 'Z', 'z'},
    {'x', 'X', 'X', 'x'},
    {'c', 'C', 'C', 'c'},
    {'v', 'V', 'V', 'v'},
    {'b', 'B', 'B', 'b'},
    {'n', 'N', 'N', 'n'},
    {'m', 'M', 'M', 'm'},
    {',', '<', ',', '<'},
    {'.', '>', '.', '>'},
    {'/', '?', '/', '?'},
    {0x00, 0x00, 0x00, 0x00}, // right shift
    {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {' ', ' ', ' ', ' '},
    {0x00, 0x00, 0x00, 0x00}, // capslock
    {0x00, 0x00, 0x00, 0x00}, // F1
    {0x00, 0x00, 0x00, 0x00}, // F2
    {0x00, 0x00, 0x00, 0x00}  // F3
};

/* print_keycode
 * Inputs:  None
 * Return Value: None
 * Function: print out the keys when interrupted to the screen. Takes care of the backspace, ctrl-L and shift and caps and sore the data in a temp buffer
 * till enter is pressed or buffer size becomes 128 */
void print_keycode(void)
{
    cli();
    uint32_t res;
    int index;      // index for keyboard_char array
    str_len = terminal[displayed_terminal].length;
    // retrieve the key code from keyboard
    volatile uint8_t command;
    res = inb(PS2STATUS);
    if ((res & READMASK) == 0)
    {
        return;
    }
    command = inb(PS2DATAPORT);

    uint8_t cmd;
    // check if command we receive from ps2 is valid AND not special
    // assign shift, shift_released and caps
    if (command == leftSHIFT + released || command == rightSHIFT + released)
    {
        // printf("reached released for shift");
        shift = 0;
        // shift_released = 1;
    }
    else if (command == ALT + released)
    {
        alt = 0;
    }
    else if (command == leftCRTL + released)
    {
        control = 0;
    }
    else if (command == leftCRTL)
    {
        control = 1;
    }
    else if (command >= TOTAL_SCANCODES || command < 1)
    {
        cmd = 0x00;
        send_eoi(keyboard_irq); // send eoi signal
        sti();
        return;
    }
    else if (command == CAPSLOCK)
    { // check if CAPSLOCK is pressed
        caps = caps ^ 1;
    }
    else if (command == leftSHIFT || command == rightSHIFT)
    { // check if SHIFT is pressed
        shift = 1;
    }
    else if (command == ALT)
    {
        alt = 1;
    }

    // assign index to the array based on status of caps and shift key
    if (shift == 0 && caps == 0)
    {
        index = 0;
    }
    else if (shift && !caps)
    {
        index = 1;
    }
    else if (!shift && caps)
    {
        index = 2;
    }
    else
    {
        index = 3;
    }

    // CRTL+L - clear the screen; also clear the keyboard buffer
    if (control)
    {
        if (command == L)
        {
            clear(); // clear screen but not the buffer
            send_eoi(keyboard_irq);
            sti();
               //test_flag = 102;
               // test_flag = 1313;
            return;
        }
    }
    if (alt)
    {
        if (command == F1)
        {
            send_eoi(keyboard_irq);
            open_terminal(first_terminal);
            sti(); // enable interrupts
           //  test_flag = 1313;
            return;
        }
        else if (command == F2)
        {
            send_eoi(keyboard_irq);
            open_terminal(second_terminal);
            sti(); // enable interrupts
            // test_flag = 1313;
            return;
        }
        else if (command == F3)
        {            
            send_eoi(keyboard_irq);
            open_terminal(third_terminal);
            sti(); // enable interrupts
           //  test_flag = 1313;
            return;
        }
    }
    cmd = keyboard_char[command][index];
    
    // storeVideomem(displayed_terminal);
    // check the ENTER key is pressed
    // print the normal char to the screen
    if (cmd != 0x00 && cmd != ENT && str_len < MAX_BUF_SIZE - 1 && cmd != BKSP)
    {
        terminal[displayed_terminal].tempbuf[str_len++] = cmd;
        terminal[displayed_terminal].length = str_len;
        terminal_putc(cmd, displayed_terminal);
    }
    else if ((cmd == ENT))
    {
        // Want to raise enter flag so terminal reads 1 line automatically
        terminal[displayed_terminal].tempbuf[str_len++] = ENT;
        terminal[displayed_terminal].length = 0;
        terminal[displayed_terminal].read_length = str_len;
       // str_len = 0;
        terminal[displayed_terminal].enter_flag = 1;
        terminal_putc(cmd, displayed_terminal);
        send_eoi(keyboard_irq);
        sti();
        return;
    }
    else if (cmd == BKSP)
    {
        if (str_len > 0)
        {              // if delete, delete the last character in tempbuffer and return
            str_len--; // pagefault occurs othrwise since str_length<0

            terminal[displayed_terminal].tempbuf[str_len] = 0;
            terminal[displayed_terminal].length = str_len;
            terminal_putc(cmd,displayed_terminal);
        }
    }
    else if (str_len == MAX_BUF_SIZE - 1)
    {
        // enter_flag = 1;
        // terminal[displayed_terminal].tempbuf[str_len] = ENT;
        // terminal[displayed_terminal].length = str_len + 1; // TA said when reaching the max_buf_size, unless pressing ENT, the terminal will not print anything
        send_eoi(keyboard_irq);
        sti();
        // test_flag = 1313;
        return;
    }

    send_eoi(keyboard_irq);
    sti(); // enable interrupts
   //  test_flag = 1313;
    return;
}

/* clear_temp_buff
 * Inputs:  None
 * Return Value: None
 * Function: clears the temp buff */
void clear_temp_buff()
{
     test_flag = 14;
    int j;
    for (j = 0; j < MAX_BUF_SIZE; j++)
    {
        terminal[displayed_terminal].tempbuf[j] = NULL;
    }
    str_len = 0;
}

/* get_len
 * Inputs:  None
 * Return Value: str_length- length of one line
 * Function: gets the length f one line  */
int get_len(void) {
    return terminal[displayed_terminal].read_length;
}

/* get_flag
 * Inputs:  None
 * Return Value: enter_flag- returns whether enter_flag is 1 or 0
 * Function: returns the status of enter_flag */
int get_flag(void) {
    return terminal[active_terminal].enter_flag;
}

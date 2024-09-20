#include "i8259.h"
#include "task.h"
#include "terminal.h"
#include "paging.h"
#include "system_calls.h"
#include "types.h"

// schedule a timer interrupt 50 milliseconds to switch to the next task in a round-robin fashion
#define SCHEDULING_FREQ     100
#define DEFAULT_PIT_FREQ    1193180         // Default interrupt frequency for pit
#define DATA_PORT_CH_0      0x40            // Channel 0 data port (read/write)
#define DATA_PORT_CH_1      0x41            // Channel 1 data port (read/write)
#define DATA_PORT_CH_2      0x42            // Channel 2 data port (read/write)
#define CMD_PORT            0x43            // Mode/Command register (write only, a read is ignored)

#define LOWER_BYTE_MASK 0xFF

// Targeting channel 0
// Need to send 16 bits, so Access mode needs to be lobyte/hibyte
// Using binary mode
// Using mode 3 to generate a Square Wave
#define PIT_CMD         0x36
#define PIT_IRQ         0x00                // PIT is connected to IRQ0

extern void init_pit();                     //initialize pit
extern void pit_change_freq(uint32_t freq); //set the interrupt frequency
extern void pit_intr(void);                 //RTC interrupt handler 

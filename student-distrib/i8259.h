/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define SLAVE_8259_PORT     0xA0

/* Data ports that each PIC sits on */
#define MASTER_8259_DATA    0x21
#define SLAVE_8259_DATA     0xA1

#define SLAVEIRQ            2           // The slave PIC is connected to IRQ 2 on master PIC

#define MASTER_STARTER      0XFF        // Initial value masks for PICs
#define SLAVE_STARTER       0xFF

// Master handles irq 0-7
#define MASTER_IQR_COUNT    8           // Max number of IRQ lines a PIC can handle

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1                0x11        // Initialise command: wait for 3 extra words on the data port
#define ICW2_MASTER         0x20        // Its vector offset. (ICW2)
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04        // Tell it how it is wired to master/slaves.
#define ICW3_SLAVE          0x02
#define ICW4                0x01        // Gives additional information about the environment. (8086/88 (MCS-80/85) mode)

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
extern void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */

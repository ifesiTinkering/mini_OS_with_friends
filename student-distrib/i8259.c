/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
/*
this code is refenrced from ece 391 lecture slides from lecuture 10: https://courses.engr.illinois.edu/ece391/sp2022/secure/sp22_lectures/ECE391_Lecture10_SP22(F).pdf
*/
#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = MASTER_STARTER;   /* IRQs 0-7  */
uint8_t slave_mask = SLAVE_STARTER;     /* IRQs 8-15 */

/* 
 * i8259_init
 *   DESCRIPTION: Initialize the 8259 PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initialize the master PIC and enables interrupt from slave PIC
 */
void i8259_init(void) {
	

    cli(); 
    outb(ICW1, MASTER_8259_PORT);           // starts the initialization sequence (in cascade mode)
	outb(ICW1, SLAVE_8259_PORT);

	outb(ICW2_MASTER, MASTER_8259_DATA);    // ICW2: Master PIC vector offset
	outb(ICW2_SLAVE , SLAVE_8259_DATA);     // ICW2: Slave PIC vector offset

	outb(ICW3_MASTER, MASTER_8259_DATA);    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(ICW3_SLAVE, SLAVE_8259_DATA);      // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(ICW4, MASTER_8259_DATA);
	outb(ICW4, SLAVE_8259_DATA);
 	
    enable_irq(SLAVEIRQ);                   // Enable interrupt from secondary PIC
    sti();
}


/* 
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ
 *   INPUTS: irq_num -- specific irq line to enable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: updates current mask
 */
void enable_irq(uint32_t irq_num) { 
    uint16_t port;
    uint8_t value;

    // Detecting invalid input
    if (irq_num < 0 || irq_num >= 16)
        return;
    
    // When irq line is on master PIC
    if(irq_num < MASTER_IQR_COUNT) {
        value = ~(1 << irq_num) & master_mask;  // Turn on the input bit
        port = MASTER_8259_DATA;
        master_mask = value;                    // Update master mask for later use
    } else {
        port = SLAVE_8259_DATA;         
        irq_num -= MASTER_IQR_COUNT;
        value = ~(1 << irq_num) & slave_mask;   // Calculate a new mask
        slave_mask= value;                      // Update slave mask for later use
    } 

    outb(value, port);                          // Write corresponding mask to corresponding port
    return;    
}


/* 
 * enable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ
 *   INPUTS: irq_num -- specific irq line to disable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: updates current mask
 */
void disable_irq(uint32_t irq_num) {

    uint16_t port;
    uint8_t value;

    if (irq_num<0 || irq_num>=16)               //boundry checks
    return;
 
    if(irq_num < MASTER_IQR_COUNT) {
        value = (1 << irq_num) | master_mask;   // Turn off the input bit 
        port = MASTER_8259_DATA;
        master_mask= value;                     //save for next time
    } 
    else 
    {
        port = SLAVE_8259_DATA;
        irq_num -= MASTER_IQR_COUNT;
        value = (1 << irq_num) | slave_mask;
        slave_mask= value;                      //save for next
    } 

    outb(value, port); 
    return;
}


/* 
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
 *   INPUTS: irq_num -- specific irq line to disable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void send_eoi(uint32_t irq_num) {
    int flag;       // 1 if for master and 0 if not
    flag = 1;
    
    // Check invalid input
    if (irq_num<0 || irq_num>=16) 
        return;

    /* irq bigger than 8, slave */
    if (irq_num >= MASTER_IQR_COUNT){
        flag = 0; 
    }

    // irq from slave PIC, issue cmd to both PIC
    if(flag==0){
        irq_num -= MASTER_IQR_COUNT;
		outb((EOI | irq_num), SLAVE_8259_PORT); //not sure how many buts this needs to be but all the significant ones are in                                          
        outb((EOI | SLAVEIRQ), MASTER_8259_PORT);
    }
    else{
	// irq from master PIC, issue cmd to only master PIC
     outb(EOI | irq_num, MASTER_8259_PORT); 
    }
    return;
}

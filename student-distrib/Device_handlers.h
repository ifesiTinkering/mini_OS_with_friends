#ifndef _DEVICE_HANDLERS_H
#define _DEVICE_HANDLERS_H

//interrupt handlers envelops
/* 
 * rtc_wrapper
 *   DESCRIPTION: calls the interrupt handler for rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: saves all registers and flags 
 */
extern void rtc_wrapper(void);

/* 
 * keyboard_wrapper
 *   DESCRIPTION: calls the interrupt handler for keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: saves all registers and flags 
 */
extern void keyboard_wrapper(void);

/* 
 * pit_wrapper
 *   DESCRIPTION: calls the interrupt handler for pit
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: saves all registers and flags 
 */
extern void pit_wrapper(void);
#endif

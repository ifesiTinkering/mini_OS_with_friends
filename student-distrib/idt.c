/*
Reference for this code: IA-32 Intel® Architecture Software Developer’s Manual Volume 3: System Programming Guid
*/
#include "idt.h"

int idt_index; // parse throught through and initialize idt table according to documentation

/* 
 * init_idt
 *   DESCRIPTION: initialize idt
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void init_idt(void)
{
    ///go through all idt entries
    for (idt_index = 0; idt_index < VECTORS_IN_IDT; idt_index++) {
        int flag14 = 0;

        //flag for 15th entry so we know not to modify it
        if (idt_index == 15)
            flag14 = 1;

        //flag for system calls so we know when we are at one
        int flag_Systemcall = 0;
        if (idt_index == VECTOR_SYSTEM)
            flag_Systemcall = 1;

        int flag_interrupts = 0;

        //flag to know if we are handliing an interrupt entry
        if (idt_index >= FIRST_INTERRUPT && idt_index <= LAST_INTERRUPT)
            flag_interrupts = 1;

        //only set to present if we are using it 
        idt[idt_index].present = 0;

         //interrupts only have this set to 1 
        if (flag_interrupts == 1)
            idt[idt_index].reserved3 = 1; 
        else
            idt[idt_index].reserved3 = 0;

        //only system calls have this set to 3
        if (flag_Systemcall == 1)
            idt[idt_index].dpl = 3; 
        else{
            idt[idt_index].dpl = 0;
        }
        
        //everything lese initilaized according to aforemnetioned manual
        idt[idt_index].reserved0 = 0;
        idt[idt_index].reserved1 = 1;
        idt[idt_index].seg_selector = KERNEL_CS;
        idt[idt_index].reserved4 = 0;
        idt[idt_index].size = 1; 
        idt[idt_index].reserved2 = 1;
    }

    //set exceptions 
    SET_IDT_ENTRY(idt[0x00], DivideError);
    SET_IDT_ENTRY(idt[0x01], DebugException);
    SET_IDT_ENTRY(idt[0x02], nmiInterrupt);
    SET_IDT_ENTRY(idt[0x03], Breakpoint);
    SET_IDT_ENTRY(idt[0x04], Overflow);
    SET_IDT_ENTRY(idt[0x05], BoundRangeExceeded);
    SET_IDT_ENTRY(idt[0x06], InvalidOpcode);
    SET_IDT_ENTRY(idt[0x07], DeviceNotAvailable);
    SET_IDT_ENTRY(idt[0x08], DoubleFault);
    SET_IDT_ENTRY(idt[0x09], CoprocessorSegmentOverun);
    SET_IDT_ENTRY(idt[0x0A], InvalidTssException);
    SET_IDT_ENTRY(idt[0x0B], SegmentNotPresent);
    SET_IDT_ENTRY(idt[0x0C], StackFaultExecption);
    SET_IDT_ENTRY(idt[0x0D], GeneralProtectionException);
    SET_IDT_ENTRY(idt[0x0E], Pagefault);
    SET_IDT_ENTRY(idt[0x10], Mathfault);
    SET_IDT_ENTRY(idt[0x11], AlignmentCheck);
    SET_IDT_ENTRY(idt[0x12], MachineCheck);
    SET_IDT_ENTRY(idt[0x13], SimdFltPtException);
    //32-255 :user-defined interupts
    
    SET_IDT_ENTRY(idt[VECTOR_SYSTEM], syscall_handler_pseudo);//not psedo anymore. now actually implemented
    SET_IDT_ENTRY(idt[VECTOR_KEYBOARD], keyboard_wrapper);
    SET_IDT_ENTRY(idt[VECTOR_RTC], rtc_wrapper);
    SET_IDT_ENTRY(idt[VECTOR_PIT], pit_wrapper);
    idt[VECTOR_SYSTEM].dpl=3;  //it was being odd so added this to make sure
    idt[VECTOR_SYSTEM].present=1;

    lidt(idt_desc_ptr); //initialize idt 
    return;
}

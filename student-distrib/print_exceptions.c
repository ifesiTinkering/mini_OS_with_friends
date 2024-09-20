#include "print_exceptions.h"
#include "lib.h"
#include "system_calls.h"

#define Error_code     256

/* handle_DivideError
 *   DESCRIPTION: prints out divide error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_DivideError(void){
	printf("Divide error handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_DebugException
 *   DESCRIPTION: prints out handle_DebugException error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_DebugException(void){
	printf("Debug exception handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_nmiInterrupt
 *   DESCRIPTION: prints out handle_nmiInterrupt error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_nmiInterrupt(void){
	printf("nmi Interrupt handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_Breakpoint
 *   DESCRIPTION: prints out handle_Breakpoint error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_Breakpoint(void){
 	printf("breakpoint  handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_Overflow
 *   DESCRIPTION: prints out handle_Overflow error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_Overflow(void){
	printf("overflow handler happening \n");
	halt_call((uint8_t) Error_code);
}


/* handle_BoundRangeExceeded
 *   DESCRIPTION: prints out handle_BoundRangeExceeded error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_BoundRangeExceeded(void){
    printf("bound range exceeded handler happening \n");
	halt_call((uint8_t) Error_code);
}


/* handle_InvalidOpcode
 *   DESCRIPTION: prints out handle_InvalidOpcode error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_InvalidOpcode(void){
	printf("invalid opcode handler happening \n");
	halt_call((uint8_t) Error_code);
}


/* handle_DeviceNotAvailable
 *   DESCRIPTION: prints out handle_DeviceNotAvailable error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_DeviceNotAvailable(void){
	printf(" Device Not Available handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_DoubleFault
 *   DESCRIPTION: prints out handle_DoubleFault error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_DoubleFault(void){
	printf(" Double Fault handler happening \n");
	halt_call((uint8_t) Error_code);
}


/* handle_CoprocessorSegmentOverun
 *   DESCRIPTION: prints out handle_CoprocessorSegmentOverun error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_CoprocessorSegmentOverun(void){
	printf(" Coprocessor Segment Overun handler happening \n");
	halt_call((uint8_t) Error_code);
}


/* handle_InvalidTssException
 *   DESCRIPTION: prints out handle_InvalidTssException error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_InvalidTssException(void){
	printf(" Invalid Tss Exception handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_SegmentNotPresent
 *   DESCRIPTION: prints out handle_SegmentNotPresent error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_SegmentNotPresent(void){
	printf(" Segment Not Present handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_StackFaultExecption
 *   DESCRIPTION: prints out handle_StackFaultExecption error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_StackFaultExecption(void){
	printf(" Stack Fault Execption handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_GeneralProtectionException
 *   DESCRIPTION: prints out handle_GeneralProtectionException error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_GeneralProtectionException(void){
	printf("  General Protection Exception handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_Pagefault
 *   DESCRIPTION: prints out handle_Pagefault error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_Pagefault(void){
	printf(" Page fault handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_Mathfault
 *   DESCRIPTION: prints out handle_Mathfault error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_Mathfault(void){
	printf(" Math fault handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_AlignmentCheck
 *   DESCRIPTION: prints out handle_AlignmentCheck error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_AlignmentCheck(void){
	printf(" Alignment Check handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_MachineCheck
 *   DESCRIPTION: prints out handle_MachineCheck error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_MachineCheck(void){
	printf(" Machine Check handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_SimdFltPtException
 *   DESCRIPTION: prints out handle_SimdFltPtException error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_SimdFltPtException(void){
	printf(" Simd Flt Pt Exception handler happening \n");
	halt_call((uint8_t) Error_code);
}

/* handle_syscall_handler_pseudo
 *   DESCRIPTION: prints out handle_syscall_handler_pseudo error text
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void handle_syscall_handler_pseudo(void){
	printf("system call happening \n ");
	halt_call((uint8_t) Error_code);
}



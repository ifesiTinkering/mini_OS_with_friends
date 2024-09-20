#ifndef PRINT_EXCEPTIONS_H
#define PRINT_EXCEPTIONS_H

//psedo handlers that just print current exception and then loop
extern void handle_DivideError(void);
extern void handle_DebugException(void);
extern void handle_nmiInterrupt(void);
extern void handle_Breakpoint(void);
extern void handle_Overflow(void);
extern void handle_BoundRangeExceeded(void);
extern void handle_InvalidOpcode(void);
extern void handle_DeviceNotAvailable(void);
extern void handle_DoubleFault(void);
extern void handle_CoprocessorSegmentOverun(void);
extern void handle_InvalidTssException(void);
extern void handle_SegmentNotPresent(void);
extern void handle_StackFaultExecption(void);
extern void handle_GeneralProtectionException(void);
extern void handle_Pagefault(void);
extern void handle_Mathfault(void);
extern void handle_AlignmentCheck(void);
extern void handle_MachineCheck(void);
extern void handle_SimdFltPtException(void);
extern void handle_syscall_handler_pseudo(void);


#endif


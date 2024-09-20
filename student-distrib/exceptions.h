#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include "system_calls.h"

//All exceptions taken from the Intel worksheet
extern void DivideError();
extern void DebugException();
extern void nmiInterrupt();
extern void Breakpoint();
extern void Overflow();
extern void BoundRangeExceeded();
extern void InvalidOpcode();
extern void DeviceNotAvailable();
extern void DoubleFault();
extern void CoprocessorSegmentOverun();
extern void InvalidTssException();
extern void SegmentNotPresent();
extern void StackFaultExecption();
extern void GeneralProtectionException();
extern void Pagefault();
extern void Mathfault();
extern void AlignmentCheck();
extern void MachineCheck();
extern void SimdFltPtException();
extern void syscall_handler_pseudo();

#endif

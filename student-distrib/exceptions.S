#define ASM 1

# globally accessable names of interrupt handlers
.global DivideError
.global SegmentNotPresent
.global DebugException
.global nmiInterrupt
.global Breakpoint
.global Overflow
.global BoundRangeExceeded
.global InvalidOpcode
.global DeviceNotAvailable
.global DoubleFault
.global CoprocessorSegmentOverun
.global InvalidTssException
.global StackFaultExecption
.global GeneralProtectionException
.global Pagefault
.global Mathfault
.global AlignmentCheck
.global MachineCheck
.global SimdFltPtException
.global syscall_handler_pseudo


# each handler pushes register values, flags, calls associated eception hanlder 
# then pops vlaues, flags then does iret to make context switch to user
DivideError:
    pushal
    pushfl
    call handle_DivideError
    popfl
    popal
    iret

DebugException:
    pushal
    pushfl
    call handle_DebugException
    popfl
    popal
    iret

nmiInterrupt:
    pushal
    pushfl
    call handle_nmiInterrupt
    popfl
    popal
    iret

Breakpoint:
    pushal
    pushfl
    call handle_Breakpoint
    popfl
    popal
    iret 

Overflow:
    pushal
    pushfl
    call handle_Overflow
    popfl
    popal
    iret

BoundRangeExceeded:
    pushal
    pushfl
    call handle_BoundRangeExceeded
    popfl
    popal
    iret

InvalidOpcode:
    pushal
    pushfl
    call handle_InvalidOpcode
    popfl
    popal
    iret

DeviceNotAvailable:
    pushal
    pushfl
    call handle_DeviceNotAvailable
    popfl
    popal
    iret

DoubleFault:
    pushal
    pushfl
    call handle_DoubleFault
    popfl
    popal
    iret

CoprocessorSegmentOverun:
    pushal
    pushfl
    call handle_CoprocessorSegmentOverun
    popfl
    popal
    iret

InvalidTssException:
    pushal
    pushfl
    call handle_InvalidTssException
    popfl
    popal
    iret

SegmentNotPresent:
    pushal
    pushfl
    call handle_SegmentNotPresent
    popfl
    popal
    iret 

StackFaultExecption:
    pushal
    pushfl
    call handle_StackFaultExecption
    popfl
    popal
    iret

GeneralProtectionException:
    pushal
    pushfl
    call handle_GeneralProtectionException
    popfl
    popal
    iret 

Pagefault:
    pushal
    pushfl
    call handle_Pagefault
    popfl
    popal
    iret 

Mathfault:
    pushal
    pushfl
    call handle_Mathfault
    popfl
    popal
    iret

AlignmentCheck:
    pushal
    pushfl
    call handle_AlignmentCheck
    popfl
    popal
    iret

MachineCheck:
    pushal
    pushfl
    call handle_MachineCheck
    popfl
    popal
    iret

SimdFltPtException:
    pushal
    pushfl
    call handle_SimdFltPtException
    popfl
    popal
    iret


# not pseudo anymore. now implemented using referencing from two documents:
# OSDev: https://wiki.osdev.org/System_Call
# given ece 391 documentation on system calls

.align 4
do_syscall_in_C:
.long ignoreThisEntry
.long halt_call
.long execute_call
.long read_call
.long write_call
.long open_call
.long close_call
.long getargs_call
.long vidmap_call
.long set_handler_call
.long sigreturn_call


.align 4
syscall_handler_pseudo:
    cli
    # Optional registers below
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esp
    
    # Registers that has to be saved 
    # All registers must be pushed, but TA hinted that using PUSHAL might not work
    pushl %ebp
    pushl %esi
    pushl %edi

    pushfl  # push flags 

    # 3 parameters pushed to stack
    pushl %edx
    pushl %ecx
    pushl %ebx    

    cmpl $1, %eax
    jl gotabadcode
    cmpl $11, %eax
    jge gotabadcode

    sti
    call *do_syscall_in_C( , %eax, 4)
    cli 
    addl    $12, %esp       # 

    popfl

    popl    %edi
    popl    %esi
    popl    %ebp
    popl    %esp
    popl    %ebx
    popl    %edx
    popl    %ecx
    sti
    iret


gotabadcode:
    movl $-1, %eax  
    addl    $12, %esp       
    popfl

    popl    %edi
    popl    %esi
    popl    %ebp
    popl    %esp
    popl    %ebx
    popl    %edx
    popl    %ecx
    sti
    iret

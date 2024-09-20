#define ASM 1
.global keyboard_wrapper
.global rtc_wrapper
.global pit_wrapper

keyboard_wrapper:
    pushal              # push registers
    pushfl              # push flags
    call print_keycode  # calling keyboard interrupt handler
    popfl               # pop flags
    popal               # pop registers
    iret                # context switch

rtc_wrapper:
    pushal
    pushfl
    call rtc_intr /*calling rtc interrupt handler*/
    popfl
    popal
    iret

pit_wrapper:
    pushal
    pushfl
    call pit_intr /*calling pit interrupt handler*/
    popfl
    popal
    iret

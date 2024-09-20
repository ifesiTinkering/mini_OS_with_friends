#ifndef _INTERRUPT_DESCRIPTOR_TABLE_H
#define _INTERRUPT_DESCRIPTOR_TABLE_H

#include "x86_desc.h"
#include "Device_handlers.h"
#include "exceptions.h"
#include "lib.h"
#include "print_exceptions.h"

#define VECTORS_IN_IDT  256 // as defined in x86_desc.h
#define VECTOR_SYSTEM   0X80
#define VECTOR_RTC      0X28
#define VECTOR_PIT      0X20
#define VECTOR_KEYBOARD 0X21
#define FIRST_INTERRUPT 0x20
#define LAST_INTERRUPT  0x2F

extern void init_idt(void);

#endif

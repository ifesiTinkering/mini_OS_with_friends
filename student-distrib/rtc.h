// Reference: https://wiki.osdev.org/RTC, ds12887 datasheet
#ifndef _RTC_DEVICE_H
#define _RTC_DEVICE_H

#include "i8259.h"

//certain ports and registers 
#define STATUS_REG_A        0x0A   //control register A is at an offset of 0x0A in CMOS RAM. Setting the x80 bit in here disables NMI. 
#define STATUS_REG_B        0x0B   //control register B is at an offset of 0x0B in CMOS RAM. Setting the x80 bit in here disables NMI. 
#define STATUS_REG_C        0x0C   //control register C is at an offset of 0x0C in CMOS RAM. Setting the x80 bit in here disables NMI. 
#define RTC_REG_NUM         0x70   //specifies the index of register and disables the NMI
#define CMOS_PORT           0x71   //read or write from/to that byte of CMOS configuration space
#define RTC_IRQ             0x08   //IRQ to enable RTC interrupt
#define BIT_6_ON            0x40   //enable PIE

#define MIN_INTR_FREQ       2
#define MAX_INTR_FREQ       1024
#define DEFAULT_FREQ        MIN_INTR_FREQ 
#define DEFAULT_HW_FREQ     1024
#define DEFAULT_HW_BIN      0x06
#define BYTES_WRITTEN       4
#define LAG                 1.0
#define DEFAULT_COUNTER     (DEFAULT_HW_FREQ/DEFAULT_FREQ)*LAG
#define DEFAULT_TARGET      DEFAULT_COUNTER
//functions 
extern void init_rtc();                 //initialize RTC 
extern void init_freq_hw(); //set the interrupt frequency
extern void change_freq_virt(int freq);             //RTC interrupt handler 
uint32_t hz_to_bin(int32_t freq);

extern int32_t rtc_open(const void* filename);
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif 

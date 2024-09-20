#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "i8259.h"
#include "terminal.h"
#include "lib.h"

#define SPECIAL_CHAR 0x61

#define keyboard_irq 0x01

/* Intialize the bit mask for ps/2 keyboard code set 1
* see the OSdev website for more infomation
* might need to change it while testing */
#define PS2DATAPORT     0x60
#define PS2STATUS       0x64
#define READMASK        0x01
#define BKSP            8
#define ENT             0X0A
#define TOTAL_SCANCODES 62
#define SHADOW          4
#define CAPSLOCK        0x3A
#define leftSHIFT       0x2A
#define rightSHIFT      0x36
#define leftCRTL        0x1D
#define TWOKEYCODE      0xE0
#define rightCRTL       0x1D
#define L               0x26
#define released        0x80
#define MAX_BUF_SIZE    128
#define F1              0x3B
#define F2              0x3C
#define F3              0x3D
#define ALT             0x38

#define first_terminal  0
#define second_terminal 1
#define third_terminal  2

/* buffer for in-line terminal */
unsigned char tempbuf[MAX_BUF_SIZE];
volatile int caps;
volatile int32_t str_len;
int32_t length;
volatile int enter_flag;

/* initialize the keyboard */
extern void keyboard_init(void);
/* print the keyboard command out */
extern void print_keycode(void);

void clear_temp_buff();
extern int get_len(void);
extern int get_flag(void);


#endif /* _I8259_H */

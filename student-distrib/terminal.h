#ifndef _TERMINAL_H
#define _TERMINAL_H


//#include "lib.c"
#include "lib.h"
#include "system_calls.h"
#include "types.h"
#include "keyboard.h"
#include "paging.h"
#include "rtc.h"

#define MAX_BUF_SIZE   128
#define MAX_TERMINALS   3

int old_terminal;
terminal_t terminal[MAX_TERMINALS];
extern int32_t terminal_read (int32_t fd, void* buf_terminal, int32_t nbytes);
extern int32_t terminal_write (int32_t fd, const void* buf_terminal, int32_t nbytes);
extern int32_t terminal_open (const void* filename);
extern int32_t terminal_close (int32_t fd);
extern int32_t donothing();
extern int open_terminal(int32_t terminal_id);
extern void restore(int32_t terminal_id);
extern void save(int32_t terminal_id);
extern void init_terminals(int32_t i);

#endif

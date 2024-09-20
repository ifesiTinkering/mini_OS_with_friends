#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "types.h"
#include "lib.h"
#include "file_system.h"
#include "x86_desc.h"
#include "task.h"
#include "paging.h"
#include "pit.h"
#include "terminal.h"

#define MAXBUFSIZE      1024
#define TRACKPIDS       4

#define FREE            0
#define OCCUPIED        1
#define SHELL_PARENT    -1
#define SHELL_PID       0

#define INVALID_INPUT   0
#define ERROR           -1
#define ENGAGED         0
#define NOT_ENGAGED     1
#define SUCCESS         0

#define PROGRAM_ADDR    0x08048000
#define NOINODE         0
#define INDEX_EMPTY     0
#define START_OF_FILE   0
#define INDEX_OCCUPIED  1

// Arbitrarily choose the first 4KB in vidmap table to be the user accessible vidmap
#define VIDMAP_TABLE_INDEX  0           
// Arbitrarily choose page that contains user accessible vidmap to be right after the user program
#define VIDMAP_PAGE_INDEX   33
// Based on VIDMAP_PAGE_INDEX, the address should be a page after the user program 
#define VIDMAP_ADDR         VIRT_ADDR_PROGRAM + FOUR_MB

/* the following are implememted system calls (exept ignoreThisEntry) */
extern int32_t ignoreThisEntry(void); //padding system call bc provided doc was not zero indexed
extern int32_t halt_call (uint8_t status);
extern int32_t execute_call (const uint8_t* command);
extern int32_t read_call (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write_call (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open_call (const uint8_t* filename);
extern int32_t close_call (int32_t fd);
extern int32_t getargs_call (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap_call (uint8_t** screen_start);
extern int32_t set_handler_call (int32_t signum, void* handler_address);
extern int32_t sigreturn_call (void);
extern int32_t get_curr_pid(void);

#endif

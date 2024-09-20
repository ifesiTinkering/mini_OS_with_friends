#ifndef _PAGING_H
#define _PAGING_H


//#include "lib.c"
#include "types.h"
#define ALIGNMENT   4096                // First 12 bits (4KB) are not used as memory address and should be skipped over

// https://wiki.osdev.org/Paging
// Both tables contain 1024 4-byte entries, making them 4 KiB each. 
// In the page directory, each entry points to a page table. 
// In the page table, each entry points to a 4 KiB physical page frame. 
// (4GB total memory space, each memory chunk is 4MB)
#define ENTRY_NUM   1024  

#define VIDEO       0xB8000             // Redefined from lib.c
#define FOUR_KB     4096
#define ONE_KB      1024
#define VIDEO_INDEX VIDEO/FOUR_KB       // The memory address of video_memory in 4KB memory unit
#define KERNEL      0x00400000          // Kernel starts at 4MB

#define MASK_20_BITS    0xFFFFF000      // MASK FOR 20 MSB to ensure all other bits are 0
#define MASK_10_BITS    0xFFC00000      // MASK FOR 10 MSB to ensure all other bits are 0

#define P       0x0001                  // Various flags the lower 12 bits represents
#define R_W     0x0002
#define U_S     0x0004
#define PWT     0x0008
#define PCD     0x0010
#define A       0x0020
#define D       0x0040
#define PS      0x0080
#define G       0x0100
#define AVL     0x0F00  

#define EMPTY_PDE   0x00000000          // Empty page directory entry, setting everything to 0

#define PD_INDEX_OFFSET         22
#define PROGRAM_IMAGE_OFFSET    0x00048000
#define EIGHT_MB        0x800000
#define EIGHT_KB        0x2000
#define FOUR_MB         0x400000

#define VIRT_ADDR_PROGRAM 0x8000000

// Setting the alignment to 4KB
uint32_t page_directory[ENTRY_NUM] __attribute__((aligned (ALIGNMENT)));
uint32_t page_table[ENTRY_NUM] __attribute__((aligned(ALIGNMENT)));
uint32_t page_table_video[ENTRY_NUM] __attribute__((aligned(ALIGNMENT)));

// Setting up page table and page directory
extern void init_paging();
void remap_video_mem_user(int32_t terminal_id);
void remap_video_mem(int32_t terminal_id);
void store_vid_mem(void);
void flush_tlb(void);
void unmap(int32_t terminal_id);
#endif


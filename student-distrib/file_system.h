#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "system_calls.h"
#include "types.h"
#include "kernel.h"
#include "lib.h"
#include "task.h"

#define BOOT_BLOCK_RESERVED     52          // By definition
#define DENTRY_RESERVED         24          // By definition

// boot_block can have at most 63 directory entries since each entry
// takes 64B and boot_block is 4KB, but boot_block also stores other info like dir_count
#define BOOT_BLOCK_MAX_DENTRY   63        
#define FILENAME_LEN            32          // file name is 32B, which is made of 4 int8_t

#define DATA_BLOCK_MAX  1023            // MAGIC NUMBER
#define ABS_BLK_SIZE    4096            // Each absolute block is 4KB
#define FD_MAX          8
#define FREE            0
#define NOTFREE         1
#define FILE_LEN        17              // (0-index) max number of files in directory

// From lecture 16 slides, page 60
typedef struct inode {
    int32_t length;
    int32_t NumDataBlk[DATA_BLOCK_MAX];
} inode_t;


typedef struct dentry {
    int8_t filename[FILENAME_LEN]; //may need to change, too small
    uint32_t filetype;
    uint32_t inode_num;
    int8_t reserved[DENTRY_RESERVED];
} dentry_t;


typedef struct boot_block {
    uint32_t dir_count;
    uint32_t inode_count;
    uint32_t data_count;
    uint8_t reserved[BOOT_BLOCK_RESERVED];
    dentry_t direntries[BOOT_BLOCK_MAX_DENTRY];
} boot_block_t;

extern unsigned int filesys_start_addr;     // starting address of file system
extern dentry_t* dentry_addr;               // starting address of dentry array
extern inode_t* inode_addr;                 // starting address of inode blocks
boot_block_t* boot_block_addr;              // starting address of boot block

extern uint32_t inodesCount;                // number of inodes in file system
extern uint32_t data_count_blk;             // number of data blocks in file system

// prints the file name of input array
extern void print_filename(int8_t* filename);
// finds the info about file with input filename and copy it to input dentry struct
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
// finds the info about file at input dentry index and copy it to input dentry struct
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
// Copies data from input inode up to the specified length
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// reads nbytes from file (indicaded by fd).
extern int32_t read_from_file(int32_t fd, void* buf, int32_t nbytes);
// writes nbytes from file (indicaded by fd), not implemented for MP3
extern int32_t write_to_file(int32_t fd, const void* buf, int32_t nbytes);
// reads one data entry 
extern int32_t read_from_directory(int32_t fd, void* buf, int32_t nbytes);
// writes one data entry 
extern int32_t write_to_directory(int32_t fd, const void* buf, int32_t nbytes);
// Resets current file index
extern int32_t open_directory(const void* buf);
// Resets current file index
extern int32_t close_directory(int32_t fd);
// writes the file of input name to buffer
extern int32_t open_file(const void* buf);
// writes the file of input name to buffer
extern int32_t close_file(int32_t fd);
#endif

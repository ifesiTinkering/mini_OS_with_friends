/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

//#define RUN_TESTS

#include "system_calls.h"
#include "terminal.h"
#include "kernel.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "debug.h"
#include "tests.h"
#include "idt.h"
#include "keyboard.h"
#include "paging.h"
#include "file_system.h"
#include "task.h"
#include "pit.h"


// Flags for handling edge case for verylargetext file
int shared_btw_terminal_fs_drivers = 0;         // Flagging the file
int large_text_blk_count = 0;                   // Count the number of blocks read


/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))

unsigned int filesys_start_addr;

unsigned int getFileAddrValue(void){
    return filesys_start_addr;
}


/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void entry(unsigned long magic, unsigned long addr) {

    multiboot_info_t *mbi;

    /* Clear the screen. */
    clear();

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%#x\n", (unsigned)magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf("flags = 0x%#x\n", (unsigned)mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG(mbi->flags, 0))
        printf("mem_lower = %uKB, mem_upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG(mbi->flags, 1))
        printf("boot_device = 0x%#x\n", (unsigned)mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG(mbi->flags, 2))
        printf("cmdline = %s\n", (char *)mbi->cmdline);

    if (CHECK_FLAG(mbi->flags, 3)) {
        int mod_count = 0;
        int i;
        module_t* mod = (module_t*)mbi->mods_addr;
        filesys_start_addr = (unsigned int)mod->mod_start;
        while (mod_count < mbi->mods_count) {
            printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
            printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
            printf("First few bytes of module:\n");
            for (i = 0; i < 16; i++) {
                printf("0x%x ", *((char*)(mod->mod_start+i)));
            }
            printf("\n");
            mod_count++;
            mod++;
        }
    }
    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
        printf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG(mbi->flags, 5)) {
        elf_section_header_table_t *elf_sec = &(mbi->elf_sec);
        printf("elf_sec: num = %u, size = 0x%#x, addr = 0x%#x, shndx = 0x%#x\n",
                (unsigned)elf_sec->num, (unsigned)elf_sec->size,
                (unsigned)elf_sec->addr, (unsigned)elf_sec->shndx);
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG(mbi->flags, 6)) {
        memory_map_t *mmap;
        printf("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
                (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
        for (mmap = (memory_map_t *)mbi->mmap_addr;
                (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
                mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
            printf("    size = 0x%x, base_addr = 0x%#x%#x\n    type = 0x%x,  length    = 0x%#x%#x\n",
                    (unsigned)mmap->size,
                    (unsigned)mmap->base_addr_high,
                    (unsigned)mmap->base_addr_low,
                    (unsigned)mmap->type,
                    (unsigned)mmap->length_high,
                    (unsigned)mmap->length_low);
    }

    /* Construct an LDT entry in the GDT */
    {
        seg_desc_t the_ldt_desc;
        the_ldt_desc.granularity = 0x0;
        the_ldt_desc.opsize      = 0x1;
        the_ldt_desc.reserved    = 0x0;
        the_ldt_desc.avail       = 0x0;
        the_ldt_desc.present     = 0x1;
        the_ldt_desc.dpl         = 0x0;
        the_ldt_desc.sys         = 0x0;
        the_ldt_desc.type        = 0x2;

        SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
        ldt_desc_ptr = the_ldt_desc;
        lldt(KERNEL_LDT);
    }

    /* Construct a TSS entry in the GDT */
    {
        seg_desc_t the_tss_desc;
        the_tss_desc.granularity   = 0x0;
        the_tss_desc.opsize        = 0x0;
        the_tss_desc.reserved      = 0x0;
        the_tss_desc.avail         = 0x0;
        the_tss_desc.seg_lim_19_16 = TSS_SIZE & 0x000F0000;
        the_tss_desc.present       = 0x1;
        the_tss_desc.dpl           = 0x0;
        the_tss_desc.sys           = 0x0;
        the_tss_desc.type          = 0x9;
        the_tss_desc.seg_lim_15_00 = TSS_SIZE & 0x0000FFFF;

        SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

        tss_desc_ptr = the_tss_desc;

        tss.ldt_segment_selector = KERNEL_LDT;
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 0x800000;
        ltr(KERNEL_TSS);
    }
    
    /* Initialize devices, memory, filesystem, enable device interrupts on the
     * PIC, any other initialization stuff... */
    // IDT initialized in boot.S
    clear();
    i8259_init();       // initialize pic
    init_paging();      // initialize paging 
    init_rtc(2);        // initialize rtc
    keyboard_init();    // initilaize keyBoard
    init_process_id_array();
    init_pit();
    
   
    // file system variables initialization
    dentry_addr = NULL;
    inode_addr = NULL;
    inodesCount = -1;
    data_count_blk = -1;

    // Should first save start of file system in kernel.c
    // read from multiboot tutorial
    // https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format

    // Parse the boot_block
    boot_block_addr = (boot_block_t *)filesys_start_addr;

    // Save the inode and data_block numbers to determine invalid input
    inodesCount = boot_block_addr->inode_count;
    data_count_blk = boot_block_addr->data_count;

    // Save the starting addresses of dentry and inode
    dentry_addr = boot_block_addr->direntries;
    inode_addr = (inode_t *)(filesys_start_addr + sizeof(boot_block_t));

    // Filling in function pointers for different file types
    // file operations table
    files_operations.open_call_entry = &open_file;
    files_operations.close_call_entry = &close_file ;
    files_operations.read_call_entry = &read_from_file;
    files_operations.write_call_entry = &write_to_file;

    // directory operations table
    directories_operations.open_call_entry = &open_directory;
    directories_operations.close_call_entry = &close_directory;
    directories_operations.read_call_entry = &read_from_directory;
    directories_operations.write_call_entry = &write_to_directory;

    // rtc operations table
    rtc_operations.open_call_entry = &rtc_open;
    rtc_operations.close_call_entry = &rtc_close;
    rtc_operations.read_call_entry = &rtc_read;
    rtc_operations.write_call_entry = &rtc_write;
    
    // stdin and stdout operations table
    stdin_operations.open_call_entry = &terminal_open;
    stdin_operations.close_call_entry = &terminal_close;
    stdin_operations.read_call_entry = &terminal_read;
    stdin_operations.write_call_entry = donothing;
    stdout_operations.open_call_entry = &terminal_open;
    stdout_operations.close_call_entry = &terminal_close;
    stdout_operations.read_call_entry = donothing;
    stdout_operations.write_call_entry = &terminal_write;

    // nothing operations table
    nothing_operations.close_call_entry=NULL;
    nothing_operations.open_call_entry=NULL;
    nothing_operations.read_call_entry= NULL;
    nothing_operations.write_call_entry= NULL;

    // Only initializing the 0th terminal when bootup
    // Other terminals will be initialized upon pit interrupts
    init_terminals(0);
    

    /* Enable interrupts */
    /* Do not enable the following until after you have set up your
     * IDT correctly otherwise QEMU will triple fault and simple close
     * without showing you any output */
    sti();


#ifdef RUN_TESTS
    /* Run tests */
    printf("Lauching tests from kernel. \n");
    launch_tests();
#else 
    printf("(TESTING FLAG CLEARED)\n");
#endif
 
    /* Execute the first program ("shell") ... */
    // unsigned char command[10] = "shell";
    // execute_call(command);
    // CP3_test();
    
    /* Spin (nicely, so we don't chew up cycles) */
    asm volatile (".1: hlt; jmp .1;");
}

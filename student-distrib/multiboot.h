/* multiboot.h - Defines used in working with Multiboot-compliant
 * bootloaders (such as GRUB)
 * vim:ts=4 noexpandtab
 */

#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#define MULTIBOOT_HEADER_FLAGS          0x00000003
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

#ifndef ASM

/* Types */
#include "types.h"

/* The Multiboot header. */
typedef struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header_t;

/* The section header table for ELF. */
typedef struct elf_section_header_table {
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} elf_section_header_table_t;

// From Boot information format
//         +-------------------+
// 0       | flags             |    (required)
//         +-------------------+
// 4       | mem_lower         |    (present if flags[0] is set)
// 8       | mem_upper         |    (present if flags[0] is set)
//         +-------------------+
// 12      | boot_device       |    (present if flags[1] is set)
//         +-------------------+
// 16      | cmdline           |    (present if flags[2] is set)
//         +-------------------+
// 20      | mods_count        |    (present if flags[3] is set)
// 24      | mods_addr         |    (present if flags[3] is set)
//         +-------------------+
// 28 - 40 | syms              |    (present if flags[4] or
//         |                   |                flags[5] is set)
//         +-------------------+
// 44      | mmap_length       |    (present if flags[6] is set)
// 48      | mmap_addr         |    (present if flags[6] is set)
//         +-------------------+

/* The Multiboot information. */
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    elf_section_header_table_t elf_sec;
    uint32_t mmap_length;
    uint32_t mmap_addr;
} multiboot_info_t;

typedef struct module {
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t string;
    uint32_t reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct memory_map {
    uint32_t size;
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} memory_map_t;

#endif /* ASM */

#endif /* _MULTIBOOT_H */

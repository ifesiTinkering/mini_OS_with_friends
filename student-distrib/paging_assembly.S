/*
 * Used OSdev - https://wiki.osdev.org/Paging 
 * This file cannot have the name paging since the .h and .c file of the same name already exists
 */

#define ASM     1

.text
.globl loadPageDirectory
.globl enablePaging


/* 
 * loadPageDirectory
 *   DESCRIPTION: prepare CR3, CR4 for paging
 *   INPUTS: page_directory -- address of page_directory array
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: writes the address of page_directory to CR3 and enable PSE in CR4
 */
loadPageDirectory:
    pushl   %ebp
    movl    %esp, %ebp
    movl    8(%esp), %eax
    movl    %eax, %cr3    /*load cr3 with the address of the page directory*/
    
    movl    %cr4, %eax    /*put the value of cr4 in the register eax*/
    orl     $0x00000010, %eax /*set PSE bit to enable  four MiB pages */
    movl    %eax, %cr4
   
    movl    %ebp, %esp
    popl    %ebp
    ret

/* 
 * enablePaging
 *   DESCRIPTION: prepare CR0 for paging
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Set the MSB and LSB of CR0
 */
enablePaging:
    pushl   %ebp
    movl    %esp, %ebp
    movl    %cr0, %eax
    orl     $0x80000001, %eax /*set the MSB, LSB of CR0 to enable paging in protected mode */
    movl    %eax, %cr0
    movl    %ebp, %esp
    popl    %ebp
    ret

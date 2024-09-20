#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "file_system.h"
#include "terminal.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER \
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure()
{
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Checkpoint 1 tests */
// Manually testing exception handler
/* Debug exception test 
 * triggers the debug exception to test the corresponding exception handler 
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */

int DebugException_test()
{
	asm volatile("int $1");
	return FAIL;
}

/* nmi Interrupt test 
 * triggers the nmi exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int nmiInterrupt_test()
{
	asm volatile("int $2");
	return FAIL;
}

/* Breakpoint test 
 * triggers the Breakpoint exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int Breakpoint_test()
{
	asm volatile("int $3");
	return FAIL;
}
/* Overflow test 
 * triggers the Overflow exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */

int Overflow_test()
{
	asm volatile("int $4");
	return FAIL;
}

/* BoundRangeExceeded test 
 * triggers the BoundRangeExceeded exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int BoundRangeExceeded_test()
{
	asm volatile("int $5");
	return FAIL;
}

/* InvalidOpcode test 
 * triggers the InvalidOpcode exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int InvalidOpcode_test()
{
	asm volatile("int $6");
	return FAIL;
}

/* DeviceNotAvailable test 
 * triggers the DeviceNotAvailable exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int DeviceNotAvailable_test()
{
	asm volatile("int $7");
	return FAIL;
}

/*  DoubleFault test 
 * triggers the  DoubleFault exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int DoubleFault_test()
{
	asm volatile("int $8");
	return FAIL;
}

/*  CoprocessorSegmentOverun test 
 * triggers the  CoprocessorSegmentOverun exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int CoprocessorSegmentOverun_test()
{
	asm volatile("int $9");
	return FAIL;
}

/* InvalidTssException test 
 * triggers the InvalidTssException exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int InvalidTssException_test()
{
	asm volatile("int $10");
	return FAIL;
}

/* SegmentNotPresent test 
 * triggers the SegmentNotPresent exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int SegmentNotPresent_test()
{
	asm volatile("int $11");
	return FAIL;
}

/* StackFaultExecption test 
 * triggers the StackFaultExecption exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int StackFaultExecption_test()
{
	asm volatile("int $12");
	return FAIL;
}

/* GeneralProtectionException test 
 * triggers the GeneralProtectionException exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int GeneralProtectionException_test()
{
	asm volatile("int $13");
	return FAIL;
}

/* Pagefault test 
 * triggers the Pagefault exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int Pagefault_test()
{
	asm volatile("int $14");
	return FAIL;
}

/* Mathfault test 
 * triggers the Mathfault exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int Mathfault_test()
{
	asm volatile("int $16");
	return FAIL;
}

/* AlignmentCheck test 
 * triggers the AlignmentCheck exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int AlignmentCheck_test()
{
	asm volatile("int $17");
	return FAIL;
}

/* MachineCheck test 
 * triggers the MachineCheck exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int MachineCheck_test()
{
	asm volatile("int $18");
	return FAIL;
}

/* SimdFltPtException test 
 * triggers the SimdFltPtException exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int SimdFltPtException_test()
{
	asm volatile("int $19");
	return FAIL;
}

/* IDT Test 
 * 
 * Asserts that first 14 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test()
{
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 14; ++i)
	{
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL))
		{
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* IDT Test - Example
 * 
 * Asserts that 16-19th IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test2()
{
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 16; i < 20; ++i)
	{
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL))
		{
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* dividezero test 
 * triggers the dividezero exception to test the corresponding exception handler
 * Inputs: None
 * Outputs:FAIL or kernel goes into while loop after executing the exception handler code 
 * Side Effects: None
 */
int dividezero()
{
	int testval = 0;
	int under_test = 1;
	testval = under_test / testval;
	testval++;
	// Manually returning fail
	return FAIL;
}

/* paging_test_1 
 * checks the pointer at memory 4 MB. Since that memory is present, it should return PASS
 * Inputs: None
 * Outputs:PASS
 * Side Effects: None
 */
// Accessing kernel space at 4 MB
int paging_test_1()
{
	TEST_HEADER;
	int *ptr = (int *)(0x400000);
	int testval = *ptr;
	testval++;
	return PASS;
}

/* paging_test_2 
 * derefernces a null pointer. This should raise a page fault exception.
 * Inputs: None
 * Outputs:kernel goes into while loop after executing the page fault exception handler code.
 * Side Effects: None
 */

int paging_test_2()
{
	TEST_HEADER;
	int *ptr = (int *)0; //random memory after 8 MB to access kernel space. Since it is not present, should generate page fault exception
	int testval = *ptr;
	testval++;
	testval++;
	return FAIL;

	// asm volatile("int $14");
}

/* paging_test_3 
 * checks the pointer at memory 8 MB. Since that memory is not present, this would trigger the page fault exception.
 * Inputs: None
 * Outputs:kernel goes into while loop after executing the page fault exception handler code.
 * Side Effects: None
 */
// Accessing right after kernel space at 8 MB
int paging_test_3()
{
	TEST_HEADER;
	int *ptr = (int *)(0x800000);
	int testval = *ptr;
	testval++;
	return FAIL;
}

/* paging_test_4
 * checks the pointer at start of video memory. Since that memory is present, it should return PASS
 * Inputs: None
 * Outputs:PASS
 * Side Effects: None
 */
// Accessing video memory
int paging_test_4()
{
	TEST_HEADER;
	int *ptr = (int *)(0xB8000);
	int testval = *ptr;
	testval++;
	return PASS;
}

/* paging_test_5
  * checks the pointer at the end of video memory page of size 4 KB. Since that memory is not present, this would trigger the page fault exception.
 * Inputs: None
 * Outputs:kernel goes into while loop after executing the page fault exception handler code.
 * Side Effects: None
 */
// Accessing right after video memory
int paging_test_5()
{
	TEST_HEADER;
	int *ptr = (int *)(4268096);
	int testval = *ptr;
	testval++;
	return FAIL;
}


/* paging_test_6
 * checks the pointer at dentry_addr. Since that memory is present, it should return PASS
 * Inputs: None
 * Outputs:PASS
 * Side Effects: None
 */ 
int paging_test_6()
{
	TEST_HEADER;
	int *ptr = (int *)(4268096);
	int testval = *ptr;
	testval++;
	return PASS;
}

/* system_calls
 * Triggers a system cal in IDT (fake for current CP's)
 * Inputs: None
 * Outputs:FAIL
 * Side Effects: None
 */ 
int system_calls()
{
	asm volatile("int $128");
	return FAIL;
}



/* Checkpoint 2 tests */
// Terminal test
/* terminal_test_read
 *   DESCRIPTION: Testing keyboard and terminal interaction
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Terminal display get changed according to keyboard input
 */
int terminal_test_read(){
	int32_t fd_test = 0;
	int32_t temp = 0x01;
	int8_t* buf_test[128];
	int32_t nbytes = 128;
	uint8_t* filename_test = (uint8_t*) temp;
	int32_t bytes_read;
	terminal_open(filename_test);
	printf("terminal drver open");
	while(1){
		bytes_read = terminal_read(fd_test, (int8_t*)buf_test, nbytes);
		terminal_write(fd_test, (int8_t*)buf_test, bytes_read);
	}
	printf("rtc interrupt happened\n");
	return PASS;
}


// RTC test
/* rtc_test_open
 *   DESCRIPTION: initizialize rtc to the default frequency
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int rtc_test_open(){
	// not needed for cp2
	printf("rtc_test_open(): starting \n");
	int32_t temp = 0x01;
	uint8_t* filename_test = (uint8_t*) temp;
	rtc_open(filename_test);
	printf("rtc_test_open(): ending \n");
	return PASS;
}

/* rtc_test_close
 *   DESCRIPTION: does nothing b/c rtc_close is not implemented in CP2
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int rtc_test_close(){
	// not needed for cp2
	return PASS;
}

/* rtc_test_read
 *   DESCRIPTION: It calls rtc_read() three times so that rtc only starts printing after all rtc_read() are called
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int rtc_test_read(){
	int32_t fd_test = 0;
	int32_t temp = 0x01;
	uint32_t* buf_test = (uint32_t*) temp;
	int32_t nbytes = 0;
	uint8_t* filename_test = (uint8_t*) temp;
	int i;
	rtc_open(filename_test);
	for(i = 0; i < 3; i++){
		printf("Waiting for rtc interrupt \n");
		rtc_read(fd_test, buf_test, nbytes);
		printf("rtc interrupt happened\n");
	}
	return PASS;
}


/* rtc_test_write_8
 *   DESCRIPTION: Change rtc interrupt frequency to 8
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int rtc_test_write_8(){
	int32_t fd_test = 0;
	uint32_t temp = 8;
	uint32_t* buf_test = &temp;
	int32_t nbytes = 0;
	
	printf("rtc_test_write(): rtc_write() starting \n");
	rtc_write(fd_test, buf_test, nbytes);
	printf("rtc_test_write(): rtc_write() ending \n");
	return PASS;
}

/* rtc_test_write_128
 *   DESCRIPTION: Change rtc interrupt frequency to 128
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int rtc_test_write_128(){
	int32_t fd_test = 0;
	uint32_t temp = 128;
	uint32_t* buf_test = &temp;
	int32_t nbytes = 0;
	
	printf("rtc_test_write(): rtc_write() starting \n");
	rtc_write(fd_test, buf_test, nbytes);
	printf("rtc_test_write(): rtc_write() ending \n");
	return PASS;
}

/* rtc_test_write_1024
 *   DESCRIPTION: Change rtc interrupt frequency to 1024
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int rtc_test_write_1024(){
	int32_t fd_test = 0;
	uint32_t temp = 1024;
	uint32_t* buf_test = &temp;
	int32_t nbytes = 0;
	
	printf("rtc_test_write(): rtc_write() starting \n");
	rtc_write(fd_test, buf_test, nbytes);
	printf("rtc_test_write(): rtc_write() ending \n");
	return PASS;
}


// File system test
// Some variables for testing the file system
dentry_t *dentry_addr = NULL;
inode_t *inode_addr = NULL;
unsigned int filesys_start_addr;
uint32_t inodesCount = -1;
uint32_t data_count_blk = -1;


/* file_system_test_init
 *   DESCRIPTION: Print out various staring addresses of the file system
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int file_system_test_init()
{
	clear();
	if (boot_block_addr != NULL)
	{
		printf("boot_block_addr PASSED \n");
		printf("boot_block_addr: %d \n", (unsigned int)boot_block_addr);
	}
	else
		printf("boot_block_addr FAILED \n");

	if (inode_addr != NULL)
	{
		printf("INNODE_ADDR PASSED\n");
		printf("INNODE_ADDR: %d \n", (unsigned int)inode_addr);
	}
	else
		printf("inode_addr FAILED\n");

	if (dentry_addr != NULL)
	{
		printf("dentry_addr PASSED\n");
		printf("dentry_addr: %d \n", (unsigned int)dentry_addr);
	}
	else
		printf("dentry_addr FAILED\n");

	if (inodesCount != -1)
	{
		printf("inodesCount PASSED\n");
		printf("dentry_addr: %d \n", inodesCount);
	}
	else
		printf("inodesCount FAILED\n");

	if (data_count_blk != -1)
	{
		printf("data_count_blk PASSED\n");
		printf("data_count_blk: %d \n", data_count_blk);
	}
	else
		printf("data_count_blk FAILED\n");

	return PASS;


}

/* test_read_by_index
 *   DESCRIPTION: Print out various attributes of the input data entry
 *   INPUTS: index -- the input index of data entry
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_by_index(int index){
	dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
	int32_t result; 
	
	//printf("calling read_dentry_by_index()\n");
	result = read_dentry_by_index(index, dentry_ptr);
	// copy failed
	if(result != 0){
		printf("Copy failed");
		return FAIL;
	}
	
	// copying first directory entry to dentry
    printf("File type is %d\n", dentry_ptr->filetype);
	printf("The inode number is %d\n", dentry_ptr->inode_num);
	print_filename(dentry_ptr->filename);

	return PASS;
}

/* test_read_by_index_16
 *   DESCRIPTION: Calls test_read_by_index() 16 times of the input data entry
 *   INPUTS: index -- the input index of data entry
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_by_index_16(){
	int index;
	for(index = 0; index <=16; index++)
		test_read_by_index(index);
	return PASS;
}


/* test_read_by_name_pass
 *   DESCRIPTION: call read_dentry_by_name with valid name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_by_name_pass(void){
    dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
	int32_t result; 
	// int i;
	printf("calling read_dentry_by_name()\n");
	result = read_dentry_by_name((const uint8_t *)"frame0.txt", dentry_ptr);
	// copy failed
	if(result != 0){
		printf("Copy failed");
		return FAIL;
	}
	// copying first directory entry to dentry
	printf("Copy successful\n");
	print_filename(dentry_ptr->filename);
    // printf("File type is %d\n", dentry_ptr->filetype);
	// while()
	print_filename(dentry_ptr->filename);
    // printf("File name is %c%c%c%c%c%c%c%c\n", dentry_ptr->filename[0], dentry_ptr->filename[1], dentry_ptr->filename[2], dentry_ptr->filename[3], dentry_ptr->filename[4], dentry_ptr->filename[5], dentry_ptr->filename[6], dentry_ptr->filename[7]);
	
	printf("The inode number is %d\n", dentry_ptr->inode_num);
	return PASS;
}

/* test_read_by_name_pass
 *   DESCRIPTION: call read_dentry_by_name with invalid name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_by_name_fail(void){

    dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
	int32_t result; 
	// int i;
	printf("calling read_dentry_by_name()\n");
	result = read_dentry_by_name((const uint8_t *)"pingp", dentry_ptr);
	// copy failed
	if(result != 0){
		printf("Copy failed");
		return FAIL;
	}
	// copying first directory entry to dentry
	printf("Copy successful\n");
	print_filename(dentry_ptr->filename);
    // printf("File type is %d\n", dentry_ptr->filetype);
	// while()
	print_filename(dentry_ptr->filename);
    // printf("File name is %c%c%c%c%c%c%c%c\n", dentry_ptr->filename[0], dentry_ptr->filename[1], dentry_ptr->filename[2], dentry_ptr->filename[3], dentry_ptr->filename[4], dentry_ptr->filename[5], dentry_ptr->filename[6], dentry_ptr->filename[7]);
	
	printf("The inode number is %d\n", dentry_ptr->inode_num);
	return PASS;
}

/* test_read_by_name_long_fail
 *   DESCRIPTION: call read_dentry_by_name with invalid name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_by_name_long_fail(void){

    dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
	int32_t result; 
	
	printf("calling read_dentry_by_name()\n");
	result = read_dentry_by_name((const uint8_t *)"verylargetextwithverylongname.txt", dentry_ptr);
	// copy failed
	if(result != 0){
		printf("Copy failed");
		return FAIL;
	}
	// copying first directory entry to dentry
	printf("Copy successful\n");
	print_filename(dentry_ptr->filename);
    // printf("File type is %d\n", dentry_ptr->filetype);
	print_filename(dentry_ptr->filename);
	
	//print_filename(dentry_ptr->filename);
    // printf("File name is %c%c%c%c%c%c%c%c\n", dentry_ptr->filename[0], dentry_ptr->filename[1], dentry_ptr->filename[2], dentry_ptr->filename[3], dentry_ptr->filename[4], dentry_ptr->filename[5], dentry_ptr->filename[6], dentry_ptr->filename[7]);
	
	printf("The inode number is %d\n", dentry_ptr->inode_num);
	return FAIL;
}

/* test_read_by_name_long_fail
 *   DESCRIPTION: call test_read_by_name_long_pass with valid name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_by_name_long_pass(void){

    dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
	int32_t result; 
	
	printf("calling read_dentry_by_name()\n");
	result = read_dentry_by_name((const uint8_t *)"verylargetextwithverylongname.tx", dentry_ptr);
	// copy failed
	if(result != 0){
		printf("Copy failed");
		return FAIL;
	}
	// copying first directory entry to dentry
	printf("Copy successful\n");
	print_filename(dentry_ptr->filename);
    // printf("File type is %d\n", dentry_ptr->filetype);
	print_filename(dentry_ptr->filename);
	// print_filename(dentry_ptr->filename);
    // printf("File name is %c%c%c%c%c%c%c%c\n", dentry_ptr->filename[0], dentry_ptr->filename[1], dentry_ptr->filename[2], dentry_ptr->filename[3], dentry_ptr->filename[4], dentry_ptr->filename[5], dentry_ptr->filename[6], dentry_ptr->filename[7]);
	
	printf("The inode number is %d\n", dentry_ptr->inode_num);
	return PASS;
}


/* test_read_by_name_long_fail
 *   DESCRIPTION: read frame0.txt into buffer and print out the buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_data(){
	uint32_t length, i;
	length = 197;
	uint8_t buf[197];
	uint32_t node;
	node = 38;
	uint32_t off;
	off= 0;
	printf("before call\n");
	printf ("number of bytes read: %d\n", read_data(node, off, buf, length));
    
	for(i=0; i<length; i++){
     //	putc(buf[i]);
	}
  
	return PASS;
}


/* test_read_by_name_long_fail
 *   DESCRIPTION: prints out the files in the directory
 *   INPUTS: index -- how many files do we want to print
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_open_dir(int index){
	int32_t fd= 0;
	int32_t buf[2]= {1,2};
	int32_t nbytes=0;
	int i;
	open_directory(buf);

	for(i = 0; i < index; i++)
		printf("#%d, read_from_dir_value: %d, ", i, read_from_directory(fd, buf, nbytes));

	return PASS;
}

/* test_read_from_file
 *   DESCRIPTION: read frame0.txt to buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_read_from_file(void){
    int32_t fd = 38;
	uint8_t buf[197];
	uint32_t nbytes, i;
	nbytes = 197;
	
	printf("%d:", read_from_file(fd, buf, nbytes));

	for(i=0; i<nbytes; i++){
     	//putc(buf[i]);
	}
	return PASS;
}



/* test_read_from_file
 *   DESCRIPTION: opening files with valid and invalid file names
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_open_file(void){
	unsigned char buf[40] = "verylargetextwithverylongname.txt";
	unsigned char buf2[40] = "fra.txt";
	unsigned char buf3[40] = "verylargetextwithverylongname.tx";

	
	
	printf("Illegal input\n");
	printf("%d\n", open_file(buf));
	
	printf("Illegal input\n");
	printf("%d\n", open_file(buf2));
	
	printf("Legal input\n");
	printf("%d\n", open_file(buf3));
	
	return PASS;
}


/* test_read_from_file
 *   DESCRIPTION: closing files with valid and invalid file names
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int test_close_file(void){
	int32_t fd;
	// unsigned char buf[40] = "verylargetextwithverylongname.txt";
	// unsigned char buf2[40] = "fra.txt";
	// unsigned char buf3[40] = "verylargetextwithverylongname.tx";

	printf("Illegal input\n");
	printf("%d\n", close_file(fd));
	
	printf("Illegal input\n");
	printf("%d\n", close_file(fd));
	
	printf("Legal input\n");
	printf("%d\n", close_file(fd));
	
	return PASS;
}


/* Checkpoint 3 tests */
void CP3_test()
{
	asm volatile("int $1");
	printf("SQUASHED");
}


/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
	// CP1: Testing exception
	// TEST_OUTPUT("Accessing beginning of kernel", AlignmentCheck_test());

	// CP1: Testing paging (should pass)
	// TEST_OUTPUT("Accessing beginning of kernel", paging_test_1());
	// TEST_OUTPUT("Accessing start of video memory", paging_test_4());
	// TEST_OUTPUT("Accessing dentry_addr", paging_test_6());

	// CP1: Testing paging (should fail)
	// TEST_OUTPUT("Accessing start of memory", paging_test_2());
	// TEST_OUTPUT("Accessing right after kernel", paging_test_3());
	// TEST_OUTPUT("Accessing right after video memory", paging_test_5());

	// CP2: Testing RTC
	// printf("launch_tests: Running rtc_test_open \n");
	// TEST_OUTPUT("rtc_test_open", rtc_test_open());
	// printf("launch_tests: Running rtc_test_close \n");
	// TEST_OUTPUT("rtc_test_close", rtc_test_close());
	// printf("launch_tests: Running rtc_test_read \n");
	// TEST_OUTPUT("rtc_test_read", rtc_test_read());
	// printf("launch_tests: Running rtc_test_write_8 \n");
	// TEST_OUTPUT("rtc_test_write_8", rtc_test_write_8());
	// printf("launch_tests: Running rtc_test_write_128 \n");
	// TEST_OUTPUT("rtc_test_write_128", rtc_test_write_128());
   	// printf("launch_tests: Running rtc_test_write_1024 \n"); 
	// TEST_OUTPUT("rtc_test_write_1024", rtc_test_write_1024());
	
	// CP2: Testing terminal and keyboard
	// TEST_OUTPUT("terminal_test_read", terminal_test_read());
	// terminal_write(0,"aaa",4);

	// CP2: Testing file system
	// TEST_OUTPUT("test_read_by_index", test_read_by_index(0));
	// TEST_OUTPUT("test_read_by_index_16", test_read_by_index_16());

	// TEST_OUTPUT("test_read_by_name_pass", test_read_by_name_pass());
	// TEST_OUTPUT("test_read_by_name_fail", test_read_by_name_fail());
	// TEST_OUTPUT("test_read_by_name_long_fail", test_read_by_name_long_fail());
	// TEST_OUTPUT("test_read_by_name_long_pass", test_read_by_name_long_pass());
	
	// TEST_OUTPUT("test_read_data", test_read_data());

	// TEST_OUTPUT("test_open_dir", test_open_dir(17));
	// TEST_OUTPUT("test_open_file", test_open_file());
	// printf ("testing output: %d ", open_file((uint8_t*)"."));
	// TEST_OUTPUT("test_close_file", test_close_file());
	// TEST_OUTPUT("test_read_from_file", test_read_from_file());
}
 


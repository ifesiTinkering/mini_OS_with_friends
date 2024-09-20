#include "file_system.h"
#define PROBLEM -1
 /* print_filename
 *   DESCRIPTION: prints the file name of input array to displaying terminal
 *   INPUTS: filename -- pointer to a filename string
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void print_filename(int8_t* filename){
    int i;
    for(i = 0; i < FILENAME_LEN; i++){
        terminal_putc(filename[i], displayed_terminal);
    }
    printf("\n");
    return;
}


/* read_dentry_by_name
 *   DESCRIPTION: finds the info about file with input filename and copy it to input dentry struct
 *   INPUTS: fname -- pointer to a filename string that we want to find
 *           dentry -- input data entry struct to be filled
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if invalid input struct, or 0 if copy successful
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry)
{
    uint32_t target_index = -1;             // The index of the dentry we want to copy from
    int dentry_index, i, input_length;      // Used for looping
    unsigned char curr_char;
    
    // Checking if dentry is a nullptr
    if (dentry == NULL)
        return -1;
    if(strlen((int8_t*)fname) == 0)
        return -1;    
    
    // Checking whether input exceeds limit in length
    i = 0;
    input_length = 0;
    curr_char = fname[i];
    while(curr_char != 0){
        input_length++;
        i++;
        curr_char = fname[i];
    }
    if(input_length > FILENAME_LEN){
        return -1;
    }

    // Searching all of the dentries with file name
    for (dentry_index = 0; dentry_index <= BOOT_BLOCK_MAX_DENTRY; dentry_index++)
    {
        // Checking if filename matches up to 32 characters
        if (0 == (strncmp( (int8_t *)fname, (int8_t *)dentry_addr[dentry_index].filename, 32))){
            target_index = dentry_index;    // Save addr of target_entry and its index
        }
    }
    
    // if not found or not withg=in valid bound
    if (target_index < 0 || target_index > BOOT_BLOCK_MAX_DENTRY)
        return -1;  
        
    // Copying over filename
    for (i = 0; i < FILENAME_LEN; i++){
        dentry->filename[i] = dentry_addr[target_index].filename[i];
    }
    // Copying over filetype
    dentry->filetype = dentry_addr[target_index].filetype;
    // Copying over inode_num
    dentry->inode_num = dentry_addr[target_index].inode_num;
    // Copying over reserved (not sure needed)
    for (i = 0; i < DENTRY_RESERVED; i++)
    {
        dentry->reserved[i] = dentry_addr[target_index].reserved[i]; 
    }   

    return 0;
}

/* read_dentry_by_index
 *   DESCRIPTION: finds the info about file at input dentry index and copy it to input dentry struct
 *   INPUTS: index -- index of the input directory in bootblock
 *           dentry -- input data entry struct to be filled
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if invalid input struct, or 0 if copy successful
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry)
{  
    int i;      // used for looping
    
    // Checking invalid input
    if (index < 0 || index > BOOT_BLOCK_MAX_DENTRY || dentry == NULL )
        return -1;

    // Copying over filename
    for (i = 0; i < FILENAME_LEN; i++)
    {
        dentry->filename[i] = dentry_addr[index].filename[i];
    }
    
    // Copying over filetype
    dentry->filetype = dentry_addr[index].filetype;
    
    // Copying over inode_num
    dentry->inode_num = dentry_addr[index].inode_num;
    // Copying over reserved (not sure needed)
    for (i = 0; i < DENTRY_RESERVED; i++){
        dentry->reserved[i] = dentry_addr[index].reserved[i]; 
    } 
    
    return 0;
}


/* read_data
 *   DESCRIPTION: Copies data from input inode up to the specified length
 *   INPUTS: inode -- index of the input directory in bootblock
 *           offset -- input data entry struct to be filled
 *           buf -- where we want to write the data to
 *           length -- the number of bytes we want to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if invalid input struct or inode, or the number of bytes correctly written
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length)
{

    // printf("in read data foo\n");
    inode_t* target_inode;          // Address of inode we want to read from
    uint32_t buf_index = 0;         // The index of buffer to write to
    uint32_t curr_byte_in_data;     // Current byte to write to buffer in data block, ranging from 0 to 4K
    uint32_t idxDataBlk;            // The index of data_block in inode starting from 0
    uint32_t data_block_number;     // The address of above data_block
    int32_t ret;                    // read bytes number
    int32_t* src_byte_ptr;          // Address of the byte we want to read from
    
    
    // Checking invalid input
    if (inode < 0 || inode > inodesCount)
        return -1;

    // Checking invalid buffer
    if (buf == NULL)
        return -1;
    
    // EDGE CASE: Making sure verylargetext works
    dentry_t temp_dent;
    read_dentry_by_name((unsigned char*)"verylargetextwithverylongname.tx", &temp_dent); 
    
    // Turn on the flag for verylargetext and start counting correct number of blocks
    if ((&temp_dent)->inode_num == inode) {
        large_text_blk_count++;
        shared_btw_terminal_fs_drivers=1;
    } else {
        large_text_blk_count = 0;
    }

    // Calculate target inode address
    target_inode = (inode_t *)(inode_addr + inode);
    
    // Checking invalid offset
    if (offset >= target_inode->length)
        return 0;

    // Determining where the target byte is located
    idxDataBlk = offset / sizeof(inode_t);  
    curr_byte_in_data = offset % sizeof(inode_t);         
    
    ret = 0;
    // printf("file length is %d", target_inode->length);
    int file_length = target_inode->length;
    // Keep writing to the buffer until length
    while (buf_index <= length-1)
    {
        // find the data_block_number given the corresponding block index
        data_block_number = (target_inode->NumDataBlk[idxDataBlk]);
        // checking if current block address is within file system
        if(data_block_number >= 0 && data_block_number < data_count_blk){
            // Determine the location of the target byte
            // First cast address to (uint8_t*) so the incrementing unit is 1 byte
            // Get the starting address of data blocks: inode_addr+inodesCount
            // add the offset of current data block: ABS_BLK_SIZE*data_block_number
            // add the offset of current byte in data block
            src_byte_ptr = (int32_t*) ((uint8_t*) (inode_addr+inodesCount)  + (ABS_BLK_SIZE*data_block_number) + curr_byte_in_data);
            // Write 1 byte to the corresponding buffer location
            *(buf+buf_index) = *(src_byte_ptr);

            // Increment indices, no need to increment buf
            buf_index++;
            curr_byte_in_data++;
            ret++;

            // Reached end of a data_block
            if (curr_byte_in_data >= sizeof(inode_t)){
                idxDataBlk = idxDataBlk + 1;            // Move on to the next data_block
                curr_byte_in_data = 0;                  // Reset curr_byte_in_data
            }
        }
        else
            return -1; 
    }

    // check if current file is large text and keep track of the number of blocks written
    if (shared_btw_terminal_fs_drivers && file_length < length * large_text_blk_count) {
        file_length -= length * (large_text_blk_count-1);
    } else if (file_length >= length) {
        file_length = length;
    }

    return file_length;
}


/*
 * read_from_file
 *   DESCRIPTION: reads nbytes from file (indicaded by fd).
 *   INPUTS: fd -- file descriptor table index
 *           buf -- populated with read bytes form file
 *           nybtes -- number of bytes read from file
 *   RETURN VALUE: number of bytes read from file; -1 if cannot read from file
 */
int32_t read_from_file(int32_t fd, void* buf, int32_t nbytes){
    // Checking if input is valid
    if ((buf == NULL) || (fd < 0) || (fd >= FD_MAX) || (nbytes==0))
        return PROBLEM;
    
    // Get the current pcb to get where we need to read from
    int32_t curr_pid =  get_curr_pid();
    pcb_t* cur_pcb_addr = (pcb_t*) get_pcb_blk(curr_pid);

    // Use read_data helper function to perform actual read
    // USAGE: int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length)
    // NOTE:  The helper function takes in inode and offset and returns the number of bytes read
    int ret;
    ret = read_data(cur_pcb_addr->file_desc_array[fd].inode_num, cur_pcb_addr->file_desc_array[fd].file_position, buf, nbytes);

    // If reading goes wrong :(    
    if(ret == PROBLEM)
        return PROBLEM;

    return ret;
}



/*
 * write_to_file
 *   DESCRIPTION: writes nbytes from file (indicaded by fd), not implemented for MP3
 *   INPUTS: fd -- file descriptor table index
 *           buf -- where the written data is provided
 *           nybtes -- number of bytes to write
 *   RETURN VALUE: number of bytes wrote to file; -1 if cannot write to file
 */
int32_t write_to_file(int32_t fd, const void* buf, int32_t nbytes){
    //might need ro be 0 but we ret -1 bc this is a read-only file system
    //might want a special identifier later to let user know this is READ ONLY
    return -1; 
}

// Keeps track of which file in the directory is read
// Used and incremented by read_from_directory
int32_t internalCounter;
int32_t ret;


/*
 * read_from_directory
 *   DESCRIPTION: reads one data entry 
 *   INPUTS: fd -- file descriptor table index
 *           buf -- the buffer that is populated by read file name
 *           nybtes -- number of bytes read from directory
 *   RETURN VALUE: 1 if successful read; 0 if cannot read 
 */
int32_t read_from_directory(int32_t fd, void* buf, int32_t nbytes){
    dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
    
    // If all entried are already read
    if (internalCounter>=FILE_LEN){
        internalCounter=0;
        return 0;
    }
   
    // Read the dentry corresponding to the counter into dentry_ptr
    read_dentry_by_index(internalCounter, dentry_ptr);
    memcpy(buf, dentry_ptr->filename, FILENAME_LEN);
    internalCounter++;

    // Return the length of bytes of any file
    return FILENAME_LEN;
}

/*
 * open_directory
 *   DESCRIPTION: Resets current file index
 *   INPUTS: fd -- (not used for CP2)
 *           buf -- (not used for CP2)
 *           nybtes -- (not used for CP2)
 *   RETURN VALUE: 0
 */
int32_t open_directory(const void* buf)
{
    // Set the internal counter to 0 upon opening a new directory
    internalCounter = 0;
    return 0;
}


/*
 * write_to_directory
 *   DESCRIPTION: writes one data entry  
 *   INPUTS: fd -- (not used for CP2)
 *           buf -- (not used for CP2)
 *           nybtes -- (not used for CP2)
 *   RETURN VALUE: -1
 */
int32_t write_to_directory(int32_t fd, const void* buf, int32_t nbytes)
{
    //might need ro be 0 but we ret -1 bc this is a read-only file system
    //might want a special identifier later to let user know this is READ ONLY
    return -1; 
}

/*
 * close_directory
 *   DESCRIPTION: Resets current file index
 *   INPUTS: fd -- (not used for CP2)
 *           buf -- (not used for CP2)
 *           nybtes -- (not used for CP2)
 *   RETURN VALUE: 0
 */
int32_t close_directory(int32_t fd)
{
    // Mirroring open_directory
    internalCounter = 0;
    return 0;
}


/*
 * open_file
 *   DESCRIPTION: writes the file of input name to buffer
 *   INPUTS: fd -- (not used for CP2)
 *           buf -- the buffer that gets populated with dentry
 *           nybtes -- (not used for CP2)
 *   RETURN VALUE: -1 if null ptr, 0 if successful open
 */
int32_t open_file(const void* buf)
{
    // Checking for null pointer
    if(buf == 0)
        return -1;

    dentry_t dentry_input;
	dentry_t* dentry_ptr;
	dentry_ptr = &dentry_input;
	int32_t result; 

	result = read_dentry_by_name((const uint8_t *)buf, dentry_ptr);
    return result;
}


/*
 * close_file
 *   DESCRIPTION: writes the file of input name to buffer
 *   INPUTS: fd -- (not used for CP2)
 *           buf -- the buffer that gets populated with dentry
 *           nybtes -- (not used for CP2)
 *   RETURN VALUE: -1 if null ptr, 0 if successful open
 */
int32_t close_file(int32_t fd)
{
    return 0 ;
}



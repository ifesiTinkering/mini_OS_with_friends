// FOLLOWING THE WEBPAGE: https://wiki.osdev.org/RTC
// USING THE DS12887 RTC DATASHEET: https://courses.grainger.illinois.edu/ece391/sp2022/secure/references/ds12887.pdf

#include "rtc.h"
#include "lib.h"

/* 
 * init_rtc
 *   DESCRIPTION: Initialize the rtc device and set hardware freqency to 1024hz
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: disable and enable NMI and setting the periodic interrupt enable bit 
 */
void init_rtc(){                                              
     cli();                                       // disable interrupt
     
     outb(STATUS_REG_B, RTC_REG_NUM);             // specifies index to register B, and disable NMI by setting the x80 bit.
     unsigned char REGB_DATA = inb(CMOS_PORT);    // read the current value of register B
     outb(STATUS_REG_B, RTC_REG_NUM);             // set the index again
     // write the previous value OR ed with 0x40. This turns on bit 6 of register B whic sets the PIE (periodic interrupt enable bit (See datasheet page 16)                                           
     outb(REGB_DATA | BIT_6_ON, CMOS_PORT);       
   
     enable_irq(RTC_IRQ);                         // enable irq
     init_freq_hw();           // change he interrupt frequency to 1024HZ

     // specifies index to register BS, and disable NMI by setting the x80 bit.
     inb(CMOS_PORT); // read the current value of register B

     // printf("Handler is triggered after %d rtc interrupts\n", rtc_target);
     sti();
}

uint32_t hz_to_bin(int32_t freq){
     switch (freq){
     // Our kernel should limit the interrupt ratte to 1024 Hz (mp3 pdf: Appendix B)
     // Maps frequency to binary that rtc accepts
     // All numbers are from ds12887 data sheet table 3
     case 1024:
          return 0x06;
     case 512:
          return 0x07;
     case 256:
          return 0x08;
     case 128:
          return 0x09;
     case 64:
          return 0x0A;
     case 32:
          return 0x0B;
     case 16:
          return 0x0C;
     case 8:
          return 0x0D;
     case 4:
          return 0x0E;
     case 2:
          return 0x0F;
     default:
          // If input is not valid, return 0
          return 0;
     }
}



/* 
 * init_freq_hw
 *   DESCRIPTION: changes the interrupt frequency of the RTC to freq 
 *   INPUTS: freq: the frequency at which you want to set the RTC interrupts
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void init_freq_hw(){    
     int old_reg_A;
     outb(STATUS_REG_A, RTC_REG_NUM);             // specifies index to register A, and disable NMI by setting the x80 bit.
     old_reg_A = inb(CMOS_PORT);                  // GET INITIAL VALUE OF STATUS REGISTER A
     outb(STATUS_REG_A, RTC_REG_NUM);             // RESET INDEX TO A in RTC_REG_NUM
     outb((old_reg_A & 0xF0) | DEFAULT_HW_BIN, CMOS_PORT);  // WRITE THE NEW RATE in the lower 4 bits of the register A (reserved for rate). See datasheet page 15)
}


/* 
 * change_freq_virt
 *   DESCRIPTION: changes the interrupt frequency of the RTC to freq 
 *   INPUTS: freq: the frequency at which you want to set the RTC interrupts
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void change_freq_virt(int freq){
     // Don't change intr frequency if invalid input
     if(hz_to_bin(freq) == 0)
          return;
     terminal[active_terminal].rtc_counter = DEFAULT_HW_FREQ/freq;                             // Clear the rtc_counter since hw frequency changed
     terminal[active_terminal].rtc_target = DEFAULT_HW_FREQ/freq;           // calculate the number of interrupts to reach before handler happens
     // printf("target is currently %d\n", rtc_target);
}


/* rtc_intr
 *   DESCRIPTION: rtc interrpt handler. Sends in the eoi signal after interrupt is handled. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: added test_interrupt() to test rtc device
 */
void rtc_intr()
{
     cli();
     int i;
     for(i = 0; i < 3; i++){
          if(terminal[i].init_flag == 1){
               terminal[i].rtc_counter--;
               if(terminal[i].rtc_counter <= 0){
                    // test_interrupts();                             // Screen flash for CP1
                    // terminal_putc('A', active_terminal);           // Adding char for CP2
                    terminal[i].rtc_flag = 0;
                    terminal[i].rtc_counter = terminal[i].rtc_target;
               }
          }
     }    
     
     
     outb(STATUS_REG_C, RTC_REG_NUM);   // select register C
     inb(CMOS_PORT);                    // throw away contents

     send_eoi(RTC_IRQ);                 // mark the end of interrupt
     sti();
}

/* 
 * rtc_open
 *   DESCRIPTION: Set rtc frequency to default frequency
 *   INPUTS: filename -- (not used in CP2)
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t rtc_open(const void* filename){
     init_freq_hw(); //set he freqency to the default frequency
     change_freq_virt(DEFAULT_FREQ);
     return 0; //return successfully
}

/* 
 * rtc_close
 *   DESCRIPTION: Does nothing in CP2
 *   INPUTS: fd -- (not used in CP2)
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t rtc_close(int32_t fd){
     return 0; //return successfully 
}


/* 
 * rtc_read
 *   DESCRIPTION: User program that calls this function waits until next rtc interrupt occurs
 *   INPUTS: fd -- (not used in CP2)
 *           buf -- (not used in CP2)
 *           nbytes -- (not used in CP2)
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
     terminal[active_terminal].rtc_flag = 1;           //set the interrupt flag 
     while(terminal[active_terminal].rtc_flag == 1);   //wait for the interrupt handler to clear the flag since interrupt_flag is volatile
     return 0;                                         //read successful

}


/* 
 * rtc_read
 *   DESCRIPTION: Changing rtc frequency
 *   INPUTS: fd -- (not used in CP2)
 *           buf -- pointer containing input frequency
 *           nbytes -- (not used in CP2)
 *   OUTPUTS: Changing rtc frequency
 *   RETURN VALUE: number of bytes written
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
     // Checking valid input
     uint32_t* buf2 = (uint32_t*)buf;

     if(buf2 == NULL)     
          return -1;               //write unsuccessful 

     int32_t freq = *buf2;         //nbytes = 4 byte integer that stores the interrupt rate in Hz

     if(hz_to_bin(freq) == 0)      //write unsuccessful 
          return -1;   

     change_freq_virt(freq);  //write the new interrupt rate 
     // printf("parsed frequency is %d\n", freq);
     return BYTES_WRITTEN;         //NUMBER OF BYTES WRITTEN 
}

/*
LoRA simultaneous multiband receiver
brs - /05/2018

*/

/****************************************
UNIX includes
****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>




/****************************************
Project specific includes
****************************************/
#include "./multi_lora_v1.h"

#include "./utilities.h"

#include "../SX1272/SX1272_registers.h"


#include "./rtc_rx8803.h"


/****************************************
defines
****************************************/

#define DEBUG





/****************************************
global variables declaration
****************************************/

FILE * fd_log;

FILE *fd_index;

int  fd_rxdone = -1;

__u32 channelSet[8];
__u32 channelClear;

struct spi_ioc_transfer xfer[1];

int i,j; // general purpose index

__u32 chanHex[8];	// 32 bit value containing REG_FRF_MSB, REG_FRF_MID & REG_FRF_LSB 

#ifdef DEBUG
int bugindex=0;
#endif


volatile unsigned *gpio;

fd_set fds;
char buffer[2];

__u32 channel;

unsigned int lastMessageIndex[8];



char filename[255] ="";

int nb_rec_bytes;
__u8 reg_val;
__u8 reg_val_table[255];
int rssi;

int rxIndex;
int rxDoneState;

/****************************************
main
****************************************/

int main(int argc, char **argv){



// test command input
if (argc != 1){
	fprintf(stdout, "usage: %s \n", argv[0]);
	exit(EXIT_FAILURE);
}// end if (argc != 1)



// continue if command OK

fprintf(stdout,"Starting configuration...\n");

gpio = map_peripherals();


hardwareSetup(SEL0, SEL1, SEL2, RX_CSPIN, RESETPIN);
calculateChannelSettings(SEL0, SEL1, SEL2);
selectChannel(0);
spiInit();

fd_rxdone = createInterruptFile(RXDONE_INT, FALLING);


// set up all the modules
// sets the 32 bit value containing REG_FRF_MSB, REG_FRF_MID & REG_FRF_LSB
chanHex[0] = 0xD84CCC; // channel 10, central freq = 865.20MHz
chanHex[1] = 0xD86000; // channel 11, central freq = 865.50MHz
chanHex[2] = 0xD87333; // channel 12, central freq = 865.80MHz
chanHex[3] = 0xD88666; // channel 13, central freq = 866.10MHz
chanHex[4] = 0xD89999; // channel 14, central freq = 866.40MHz
chanHex[5] = 0xD8ACCC; // channel 15, central freq = 866.70MHz
chanHex[6] = 0xD8C000; // channel 16, central freq = 867.00MHz
chanHex[7] = 0xD90000; // channel 16, central freq = 868.00MHz


for(i=0; i<RXCOUNT; i++){

fprintf(stdout,"Configuring receiver #%d:\n", i);

selectChannel(i);


// module in lora mode
writeRegister(REG_OP_MODE, FSK_SLEEP_MODE); // to change  bit 7!!
writeRegister(REG_OP_MODE, LORA_SLEEP_MODE);

// initialize  modules (TX & RX) but NOT the channel
initModule();

setLoraChannel(i);
fprintf(stdout, "Channel REG_FRF = 0x%08X  ", getLoraChannel());

printLoraChannelToStdout(getLoraChannel());



// interruptions configuration
// initModule() unmasks ALL interruptions by default
writeRegister(REG_DIO_MAPPING1, 0x01); // DIO0=RxDone, DIO1=RxTimeout, DIO2=FhssChangeChannel, DIO3=ValidHeader
writeRegister(REG_DIO_MAPPING2, 0x00); // default values

writeRegister(REG_IRQ_FLAGS, 0xff);  // clears all IRQs

// datasheet step 1: FifoRxBaseDddress -> FifoAddrPtr
writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_BASE_ADDR));

//datasheet Step2: done in init

//datasheet Step3: RX continuous mode
writeRegister(REG_OP_MODE, LORA_RX_CONTINUOUS_MODE);
usleep(100000); // delay necessary to start oscillator & PLL  (SLEEP -> other mode)

lastMessageIndex[i]=0;

//create the log file & index file of current channel
sprintf(filename, "/tmp/channel%2d.log",10+i);
if ((fd_log = fopen(filename, "w+")) < 0) {
	perror("log file:");
	exit(EXIT_FAILURE);
}
fprintf(fd_log,"%09u: No message received yet!\n", lastMessageIndex[i]);
fclose(fd_log);

sprintf(filename, "/tmp/channel%2d.index",10+i);
if ((fd_index = fopen(filename, "w+")) < 0) {
	perror("index file:");
	exit(EXIT_FAILURE);
}
fprintf(fd_index,"%09u\n", lastMessageIndex[i]);
fclose(fd_index);

fprintf(stdout,"... done\n");

} // end for(i=0; i<RXCOUNT; i++)

fprintf(stdout,"Configuration completed, waiting for messages...\n");

sleep(1);

// rewinds the file (necessary for next reading).
lseek(fd_rxdone, 0, SEEK_SET);
//reads GPIO value (2 bytes).
if (read(fd_rxdone, & buffer, 2) != 2) {
perror("read");
}



// infnite loop
while(1){

//datasheet Step4: Checking reception of a packet

// Resets the table of the events we are waiting for
FD_ZERO(& fds);
	
// adds the file descriptor of RXdone interruption
FD_SET(fd_rxdone, & fds);

#ifdef DEBUG
fprintf(stdout,"step %d: attente int\n",++bugindex);
#endif

 // passive waiting for RXdone interruption (no timeout, therefore infinite waiting...)
if (select(fd_rxdone+1, NULL, NULL, & fds, NULL) < 0) {
	perror("select");
	break;
}

#ifdef DEBUG
fprintf(stdout,"step %d: int recue\n",++bugindex);
#endif

// rewinds the file (necessary for next reading).
lseek(fd_rxdone, 0, SEEK_SET);
//reads GPIO value (2 bytes).
if (read(fd_rxdone, & buffer, 2) != 2) {
perror("read");
break;
}

// at least one message received, scanning receivers (optimised)
rxIndex=0;
rxDoneState = digitalRead(RXDONE_INT);
#ifdef DEBUG
fprintf(stdout,"step %d rxDoneState= %d\n",++bugindex, rxDoneState);
#endif
while(! digitalRead(RXDONE_INT)){ 

getReceiver(rxIndex);

if(rxIndex<RXCOUNT-1){
rxIndex++;
} else {
rxIndex=0;
}// end if(rxIndex<RXCOUNT-1)

}// end while(! digitalRead(RXDONE_INT))

rxDoneState = digitalRead(RXDONE_INT);
#ifdef DEBUG
fprintf(stdout,"step %d rxDoneState= %d\n",++bugindex, rxDoneState);
#endif

#ifdef DEBUG
fprintf(stdout,"step %d:**************************************\n",++bugindex);
#endif




} // end while(1) infinite loop




return 0;

} // end main



#include "./utilities.cpp"

#include "./rtc_rx8803.cpp"





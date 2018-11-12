/*
LoRa one band transmitter
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
#include <signal.h>
#include <sys/time.h>

#include "../arduino2rpi/SPI.h"

/****************************************
Project specific includes
****************************************/
#include "./multi_lora_v1c.h"
#include "./tx_utilities.h"
#include "../SX1272/SX1272_registers.h"
#include "./rtc_rx8803.h"


/****************************************
defines
****************************************/
#define DEFAULT_OUTPUT_POWER 			2
#define DEFAULT_BANDWIDTH 				125
#define DEFAULT_CODING_RATE 			0.5
#define DEFAULT_INPLICIT_HEADER_MODE 	false
#define DEFAULT_RX_PAYLOAD_CRC 			true
#define DEFAULT_SPREADING_FACTOR 		12
#define DEFAULT_PLENGTH_MSB 			0x00
#define DEFAULT_PLENGTH_LSB 			0x08
#define DEFAULT_SYNC_WORD 				0x12

/****************************************
global variables declaration
****************************************/
__u32 channelSet[8];
__u32 channelClear;



int i,j; // general purpose index

__u32 chanHex[8];	// 32 bit value containing REG_FRF_MSB, REG_FRF_MID & REG_FRF_LSB 

#ifdef DEBUG
int bugindex=0;
#endif


volatile unsigned *gpio;

//fd_set fds;
//char buffer[2];

__u32 channel;

uint lastMessageIndex[8];



char filename[255] ="";

int nb_trans_bytes;
__u8 regVal;
//__u8 reg_val_table[255];
//int rssi;

//int rxIndex;
int txDoneState;
int txChannel;
int payloadLength;
char txMsg[255] ="";
char txMsg0[]="BEACON_Channel10";
char txMsg1[]="BEACON_Channel11";
char txMsg2[]="BEACON_Channel12";
char txMsg3[]="BEACON_Channel13";
char txMsg4[]="BEACON_Channel14";
char txMsg5[]="BEACON_Channel15";
char txMsg6[]="BEACON_Channel16";
char txMsg7[]="BEACON_Channel17";
uint  txMsgMaxLen = 128;

/****************************************
signal handler(s)
****************************************/

/****************************************
main
****************************************/

int main(int argc, char **argv){

// LoRa Parameters
uint op_dbm = DEFAULT_OUTPUT_POWER;
uint bw_khz = DEFAULT_BANDWIDTH;
double cr = DEFAULT_CODING_RATE;
bool ih_mode = DEFAULT_INPLICIT_HEADER_MODE;
bool rx_payload = DEFAULT_RX_PAYLOAD_CRC;
uint sf = DEFAULT_SPREADING_FACTOR;
uint pl_MSB = DEFAULT_PLENGTH_MSB;
uint pl_LSB = DEFAULT_PLENGTH_LSB;
uint sw = DEFAULT_SYNC_WORD;

// test command input
if (argc < 2) {
	fprintf(stdout, "Usage: %s <channel number10..17> <message (%d characters max)>\n", argv[0],txMsgMaxLen);
	exit(EXIT_FAILURE);
}

if (sscanf(argv[1], "%d", & txChannel) != 1) {
	exit(EXIT_FAILURE);
}

if((txChannel < 10) || (txChannel > 17)) {
	fprintf(stdout,"Channel number out of range, exiting...\n");
	exit(EXIT_FAILURE);
}

if (argc > 2){
	if (strlen(argv[2]) > txMsgMaxLen) {
	fprintf(stdout,"Message larger than %d characters, exiting...\n", txMsgMaxLen);
	exit(EXIT_FAILURE);
	}
	sprintf(txMsg, "%s", argv[2]);
}

if ((op_dbm < 2) || (op_dbm < 17)) {
	fprintf(stdout,"Output power out of range, exiting...\n");
	exit(EXIT_FAILURE);
}

if ((bw_khz != 125) || (bw_khz != 250) || (bw_khz != 500)) {
	fprintf(stdout,"Bandwidth out of range, exiting...\n");
	exit(EXIT_FAILURE);
}

if ((sf < 6) || (sf > 12)) {
	fprintf(stdout, "Spreading factor out of range, exiting...\n");
	exit(EXIT_FAILURE);
}

if ((pl_MSB < 6) || (pl_MSB > 65535)) {
	fprintf(stdout, "MSB preamble length out of range, exiting...\n");
	exit(EXIT_FAILURE);
}

if ((pl_LSB < 6) || (pl_LSB > 65535)) {
	fprintf(stdout, "LSB preamble length out of range, exiting...\n");
}



// continue if command OK




fprintf(stdout,"Starting TX configuration for channel %d...\n", txChannel);

gpio = map_peripherals();

spi.begin(SPI0);

pinMode(TX_RESETPIN, INPUT);

// modules reset 
pinMode(TX_RESETPIN, OUTPUT);
digitalWrite(TX_RESETPIN, HIGH);
usleep(50000);
pinMode(TX_RESETPIN, INPUT);
usleep(50000);

// sets the 32 bit value containing REG_FRF_MSB, REG_FRF_MID & REG_FRF_LSB
chanHex[0] = 0xD84CCC; // channel 10, central freq = 865.20MHz
chanHex[1] = 0xD86000; // channel 11, central freq = 865.50MHz
chanHex[2] = 0xD87333; // channel 12, central freq = 865.80MHz
chanHex[3] = 0xD88666; // channel 13, central freq = 866.10MHz
chanHex[4] = 0xD89999; // channel 14, central freq = 866.40MHz
chanHex[5] = 0xD8ACCC; // channel 15, central freq = 866.70MHz
chanHex[6] = 0xD8C000; // channel 16, central freq = 867.00MHz
chanHex[7] = 0xD90000; // channel 16, central freq = 868.00MHz

// module in lora mode
writeRegister(REG_OP_MODE, FSK_SLEEP_MODE); // to change  bit 7!!
writeRegister(REG_OP_MODE, LORA_SLEEP_MODE);
writeRegister(REG_OP_MODE, LORA_STANDBY_MODE); // to fill-in the fifo
usleep(100000);

// initialize  modules (TX & RX) but NOT the channel
initModule();

setLoraChannel(txChannel-10);
//setLoraChannel(0);

fprintf(stdout, "Channel REG_FRF = 0x%08X  ", getLoraChannel());

printLoraChannelToStdout(getLoraChannel());
fprintf(stdout,"\n");

//build the message: if nothing was provided, fills with BEACON message
if(strlen(txMsg) == 0){
	switch(txChannel-10){
	case 0:
	strcpy((char*)txMsg, (char*)txMsg0);
	break;
	case 1:
	strcpy((char*)txMsg, (char*)txMsg1);
	break;
	case 2:
	strcpy((char*)txMsg, (char*)txMsg2);
	break;
	case 3:
	strcpy((char*)txMsg, (char*)txMsg3);
	break;
	case 4:
	strcpy((char*)txMsg, (char*)txMsg4);
	break;
	case 5:
	strcpy((char*)txMsg, (char*)txMsg5);
	break;
	case 6:
	strcpy((char*)txMsg, (char*)txMsg6);
	break;
	case 7:
	strcpy((char*)txMsg, (char*)txMsg7);
	break;
	}
	payloadLength = 16;
} else {
	payloadLength = strlen(txMsg);
}

#ifdef DEBUG
fprintf(stdout,"step %d: txMsg= %s\n",++bugindex, txMsg);
#endif


// datasheet step 1: loads the FIFO
writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_TX_BASE_ADDR)); //FifoRxBaseDddress -> FifoAddrPtr
writeRegister(REG_PAYLOAD_LENGTH_LORA, payloadLength); // defines payload length

for (i=0; i<payloadLength; i++){

writeRegister(REG_FIFO, txMsg[i]);

}

fprintf(stdout,"Configuration completed, transmitting message...");


//datasheet Step2: TX  mode
writeRegister(REG_OP_MODE, LORA_TX_MODE);
usleep(100000); // delay necessary to start oscillator & PLL  (SLEEP -> other mode)

#ifdef DEBUG
fprintf(stdout,"step %d: attente TXDONE\n",++bugindex);
#endif


//datasheet Step3: wait for TX completion
	int status;
while((status = bitRead(readRegister(REG_IRQ_FLAGS),3)) == 0){
//	fprintf(stdout,"reg_irq_flag = 0x%02x\n", status);
}
	
// reset of interruption
writeRegister(REG_IRQ_FLAGS, 0xff);  

fprintf(stdout,"TX completed\n");


return EXIT_SUCCESS;

}



#include "./tx_utilities.cpp"

#include "./rtc_rx8803.cpp"




/* 
LoRA simultaneous multiband receiver
brs - /05/2018

use with rx8803 RTC
backcup of log files 

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
#include <sys/param.h>

#include "../arduino2rpi/SPI.h"

/****************************************
Project specific includes
****************************************/
#include "./multi_lora_v1d.h"
#include "./rx_utilities.h"
#include "../SX1272/SX1272_registers.h"
#include "./rtc_rx8803.h"
SPI spi;

/****************************************
defines
****************************************/
#define DEFAULT_LNA_GAIN 				0
#define DEFAULT_BANDWIDTH 				125
#define DEFAULT_CODING_RATE 			0.5
#define DEFAULT_INPLICIT_HEADER_MODE 	false
#define DEFAULT_RX_PAYLOAD_CRC 			true
#define DEFAULT_SPREADING_FACTOR 		12
#define DEFAULT_PLENGTH_MSB 			0xff
#define DEFAULT_PLENGTH_LSB 			0xff
#define DEFAULT_SYNC_WORD 				0x12

/****************************************
global variables declaration
****************************************/
FILE * fd_log;
FILE *fd_index;

FILE *fd_backups;
int  fd_rxdone = -1;
int  fd_validheader = -1;

__u32 channelSet[8];
__u32 channelClear;

int i,j; // general purpose index
__u32 chanHex[8];	// 32 bit value containing REG_FRF_MSB, REG_FRF_MID & REG_FRF_LSB 

#ifdef DEBUG2
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
int rxDoneState, validHeaderState;
int rxCount, rxTimeout;
int backupTimeHour, backupTimeMin;
int fd_alarm = -1;
int fd_i2c;	// for I2C
char command[200];
char date[50];
char file[50];
char orgfile[50];
char bakfile[50];

/****************************************
signal handler(s)
****************************************/

/****************************************
main
****************************************/
int main(int argc, char **argv){
// LoRa Parameters
int lg_db = DEFAULT_LNA_GAIN;
unsigned int bw_khz = DEFAULT_BANDWIDTH;
double cr = DEFAULT_CODING_RATE;
bool ih_mode = DEFAULT_INPLICIT_HEADER_MODE;
bool rx_payload = DEFAULT_RX_PAYLOAD_CRC;
unsigned int sf = DEFAULT_SPREADING_FACTOR;
unsigned char pl_MSB = DEFAULT_PLENGTH_MS;
unsigned char pl_LSB = DEFAULT_PLENGTH_LS;
unsigned char sw = DEFAULT_SYNC_WORD;

//	signal(SIGALRM, sighandler);


// test command input
if (argc != 5){
	fprintf(stdout, "usage: %s <number of channels> <timeout in us> <backup time (hours) <backup time (min)>\n", argv[0]);
	exit(EXIT_FAILURE);
}// end if (argc != 1)

if (sscanf(argv[1], "%d", & rxCount) != 1) exit(EXIT_FAILURE);
if (sscanf(argv[2], "%d", & rxTimeout) != 1) exit(EXIT_FAILURE);
if (sscanf(argv[3], "%d", & backupTimeHour) != 1) exit(EXIT_FAILURE);
if (sscanf(argv[4], "%d", & backupTimeMin) != 1) exit(EXIT_FAILURE);
	
// continue if command OK

	#ifdef DEBUG1
	fprintf(stdout,"step %d: heure: %d min: %d\n",++bugindex, backupTimeHour, backupTimeMin);
	#endif

fprintf(stdout,"Starting configuration...\n");
fprintf(stdout, "LNA Gain: %d\n", atoi(argv[0]));
fprintf(stdout, "Bandwidth: %d\n", atoi(argv[1]));
fprintf(stdout, "Coding Rate: %d\n", atoi(argv[2]));
fprintf(stdout, "Inplicit Header Mode: %s\n", argv[3]);
fprintf(stdout, "Rx Payload Crc: %s\n", argv[4]);
fprintf(stdout, "Spreading Factor: %d\n", atoi(argv[5]));
fprintf(stdout, "MSB Preamble Length: %d\n", atoi(argv[6]));
fprintf(stdout, "LSB Preamble Length: %d\n", atoi(argv[7]));
fprintf(stdout, "Sync Word: %d\n", atoi(argv[8]));

gpio = map_peripherals();


hardwareSetup(SEL0, SEL1, SEL2, RX_RESETPIN);
calculateChannelSettings(SEL0, SEL1, SEL2);
selectChannel(0);

	spi.begin(SPI1);

//fd_rxdone = createInterruptFile(RXDONE_INT, FALLING);
fd_validheader = createInterruptFile(VALIDHEADER_INT, FALLING);

fd_alarm = createInterruptFile(RTC_ALARM, FALLING);
	
// setting alarm time

// Accessing the bus
fd_i2c = open(I2C_BUS, O_RDWR);
if (fd_i2c < 0) {
	perror(I2C_BUS);
	exit(EXIT_FAILURE);
	}
	
	
// enabling interrup for AIE (CSEL1 = 0, CSEL0 = 0, TIE = 0)
rtc_write_reg(fd_i2c, RTC_ADDRESS,CTL, 0x08);
// reset AIE
rtc_write_reg(fd_i2c, RTC_ADDRESS,FLAG,0x00);
// fixed-cycle timer off
rtc_write_reg(fd_i2c, RTC_ADDRESS,EXT,0x00);	
// setting alarm 
rtc_write_reg(fd_i2c, RTC_ADDRESS,MIN_A,int2bcd(backupTimeMin));
rtc_write_reg(fd_i2c, RTC_ADDRESS,HOUR_A,int2bcd(backupTimeHour));
rtc_write_reg(fd_i2c, RTC_ADDRESS,WEEKDAY_A,0x80);
	
		#ifdef DEBUG1
	fprintf(stdout,"step %d: heure: %d min: %d\n",++bugindex, bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,HOUR_A)), bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MIN_A)));
	#endif

	
close(fd_i2c);


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


for(i=0; i<rxCount; i++){

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
//		rtc_write_reg(fd_i2c, RTC_ADDRESS,FLAG,0x00);
		
		close(fd_i2c);


// interruptions configuration
// initModule() unmasks ALL interruptions by default
writeRegister(REG_DIO_MAPPING1, 0x01); // DIO0=RxDone, DIO1=RxTimeout, DIO2=FhssChangeChannel, DIO3=ValidHeader
writeRegister(REG_DIO_MAPPING2, 0x00); // default values

writeRegister(REG_IRQ_FLAGS_MASK, 0x8f);  // only Rxdone, CRC error , valid header enabled

writeRegister(REG_IRQ_FLAGS, 0xff);  // clears all IRQs

// datasheet step 1: FifoRxBaseDddress -> FifoAddrPtr
writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_BASE_ADDR));

//datasheet Step2: done in init

//datasheet Step3: RX continuous mode
writeRegister(REG_OP_MODE, LORA_RX_CONTINUOUS_MODE);
usleep(100000); // delay necessary to start oscillator & PLL  (SLEEP -> other mode)

lastMessageIndex[i]=0;
	
// creates the backup log file
			if ((fd_backups = fopen("/home/lora/backups/backups.log", "w+")) < 0) {
		perror("log file:");
		exit(EXIT_FAILURE);
		}

		fclose(fd_backups);

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

} // end for(i=0; i<rxCount; i++)

fprintf(stdout,"Configuration completed, waiting for messages...\n");

sleep(1);

// rewinds the file (necessary for next reading).
lseek(fd_validheader, 0, SEEK_SET);

if (read(fd_validheader, & buffer, 2) != 2) { //reads GPIO value (2 bytes).
perror("read");
}

lseek(fd_alarm, 0, SEEK_SET);

if (read(fd_alarm, & buffer, 2) != 2) { //reads GPIO value (2 bytes).
perror("read");
}	
	
	
	
//  IRQ reset for all receivers anyway to avoid lock at startup if int pin is already low
// side effect: messages sent before are lost 
for(i=0; i<rxCount; i++){

selectChannel(i);

writeRegister(REG_IRQ_FLAGS, 0xff);  


} // end for(i=0; i<rxCount; i++)



// infnite loop
while(1){

//datasheet Step4: Checking reception of a packet

// Resets the table of the events we are waiting for
FD_ZERO(& fds);
	
// adds the file descriptor of validheader interruption
FD_SET(fd_validheader, & fds);
FD_SET(fd_alarm, & fds);

#ifdef DEBUG1
fprintf(stdout,"step %d: attente int\n",++bugindex);
#endif

 // passive waiting for RXdone interruption (no timeout, therefore infinite waiting...)
if (select(MAX(fd_validheader, fd_alarm)+1, NULL, NULL, & fds, NULL) < 0) {
	perror("select");
	break;
}


	
	if (FD_ISSET(fd_validheader, & fds)){
		
	#ifdef DEBUG1
	fprintf(stdout,"step %d: message reçu\n",++bugindex);
	#endif

	// rewinds the file (necessary for next reading).
	lseek(fd_validheader, 0, SEEK_SET);
	if (read(fd_validheader, & buffer, 2) != 2) { 	//reads GPIO value (2 bytes).
	perror("read");
	break;
	}

	// at least one message received, scanning receivers (optimised)
	rxIndex=0;



	while(! digitalRead(VALIDHEADER_INT)){ 

	getReceiver2(rxIndex, rxTimeout); // new strategy to avoid blocking

	if(rxIndex<rxCount-1){
	rxIndex++;
	} else {
	rxIndex=0;
	}// end if(rxIndex<rxCount-1)

	}// end while(! digitalRead(VALIDHEADER_INT))



	} //end if (FD_ISSET(fd_validheader, & fds))

	if (FD_ISSET(fd_alarm, & fds)){
		
	#ifdef DEBUG1
	fprintf(stdout,"step %d: alarme reçu\n",++bugindex);
	#endif
		

		
	
		
		fd_i2c = open(I2C_BUS, O_RDWR);
		if (fd_i2c < 0) {
		perror(I2C_BUS);
		exit(EXIT_FAILURE);
		}
		fprintf(stdout,"Backup in progress...\n");
		
		sprintf(date,"_date%02d-%02d-%02dtime%02d-%02d-%02d",bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,DAY)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MONTH)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,YEAR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,HOUR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MINU)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,SEC)));

		if ((fd_backups = fopen("/home/lora/backups/backups.log", "a+")) < 0) {
		perror("log file:");
		exit(EXIT_FAILURE);
		}
			fprintf(fd_backups, "Backup at: %s \n", date);
		fclose(fd_backups);
		
		rxIndex=0;
		
		while(rxIndex < rxCount){
			
		strcpy(orgfile, "/tmp/");
		sprintf(file,"channel%02d.log", rxIndex+10);
		//strcpy(file, "channel10.log");
		strcat(orgfile, file); //  gives /tmp/channelXX.log
		
		strcpy(bakfile, "/home/lora/backups/");
		strcat(bakfile,file);
		strcat(bakfile, date);	// gives /home/lora/backups/channelXX.log_YYYYYYY
		
		strcpy(command,"mv ");
		strcat(command, orgfile);
		strcat(command," ");
		strcat(command, bakfile);
			
			
		#ifdef DEBUG2
		fprintf(stdout,"debug index= %d   %s\n",bugindex++, command);
		#endif			
		
		system(command);
			
			
	sprintf(filename, "/tmp/channel%2d.log",10+rxIndex);
	if ((fd_log = fopen(filename, "w+")) < 0) {
		perror("new log file after backup:");
		exit(EXIT_FAILURE);
	}
	fprintf(fd_log,"%09u: No message received yet, previous messages in backup file(s)\n", lastMessageIndex[i]);
	fclose(fd_log);
			
			
			
			
			
		rxIndex++;
		} // end while(rxIndex < rxCount)
		
		#ifdef DEBUG2
		fprintf(stdout,"debug index= %d\n",bugindex++);
		#endif		
		
		sleep(65); // because no SEC_A register!!
		

		rtc_write_reg(fd_i2c, RTC_ADDRESS,FLAG,0x00);
		
		#ifdef DEBUG2
		backupTimeMin=backupTimeMin+2;
		rtc_write_reg(fd_i2c, RTC_ADDRESS,MIN_A,int2bcd(backupTimeMin)); // !!! FOR TESTS ONLY
		fprintf(stdout,"debug index= %d nextsave = %d\n",bugindex++, bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MIN_A)));
		#endif		
		
		close(fd_i2c);
		
				// rewinds the file (necessary for next reading).
	lseek(fd_alarm, 0, SEEK_SET);
	if (read(fd_alarm, & buffer, 2) != 2) { 	//reads GPIO value (2 bytes).
	perror("read");
	break;			
	}	
				fprintf(stdout,"Backup done\n");
	} // end if (FD_ISSET(fd_alarm, & fds))

} // end while(1) infinite loop




return 0;

} // end main



#include "./rx_utilities.cpp"

#include "./rtc_rx8803.cpp"




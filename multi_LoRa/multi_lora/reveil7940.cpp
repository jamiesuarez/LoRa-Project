/*
testing RTC
brs - /11/2016

*/

#include "/home/tom/libraries/arduino2rpi/MCP7940.h"

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
//#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/select.h>


#define I2C_BUS		"/dev/i2c-1"

// for RTC
#define	RTC_ADDRESS	0x32
#define SEC		0x00
#define MIN		0x01
#define HOUR		0x02
#define WEEK		0x03
#define	DAY		0x04
#define	MONTH		0x05
#define	YEAR		0x06
#define	RAM		0x07
#define	MIN_A		0x08
#define	HOUR_A		0x09
#define	WEEKDAY_A	0x0A
#define	TC0		0x0B
#define	TC1		0x0C
#define	EXT		0x0D
#define	FLAG		0x0E
#define	CTL		0x0F

// for TCN75A temp. sensor
#define	TCN_ADDRESS	0x48
#define TEMP_REG	0x00
#define CONF_REG	0x01
#define HYST_REG	0x02
#define SET_REG		0x03 

*/

// prototypes
//int rtc_write_reg(int, int, int, int);
//int rtc_read_reg(int, int, int);
int int2bcd(int);
int bcd2int(int);
//float read_temperature(int, int, int);


MCP7940 rtc(0x6f);

int main(int argc, char * argv[])
{

//int fd_i2c;	// for I2C
//FILE *fd1; // for GPIO
//int fd_select; // for select()
//fd_set fds;
//char buffer[2];
	
	rtc.writeRegister(SEC, 0x80);
		rtc.writeRegister(CONTROL, 0x43);
	fprintf(stdout,"SEC = %02x\n", rtc.readRegister(SEC));
	
	fprintf(stdout,"time= %d:%d:%d (hh:mm:ss)\n",bcd2int(rtc.readRegister(HOUR)),bcd2int(rtc.readRegister(MIN)),bcd2int(0x7f & rtc.readRegister(SEC)) );
	fprintf(stdout,"WKDAY = %02x\n", rtc.readRegister(WKDAY));
	
		rtc.writeRegister(SEC, (int2bcd(55) | 0x80));
	
	fprintf(stdout,"time= %d:%d:%d (hh:mm:ss)\n",bcd2int(rtc.readRegister(HOUR)),bcd2int(rtc.readRegister(MIN)),bcd2int(0x7f & rtc.readRegister(SEC)) );
	
sleep(5);
		fprintf(stdout,"time= %d:%d:%d (hh:mm:ss)\n",bcd2int(rtc.readRegister(HOUR)),bcd2int(rtc.readRegister(MIN)),bcd2int(0x7f & rtc.readRegister(SEC)) );
	
	
/*
// creating GPIO4 as input port, select falling edge for interruption event (GPIO4 is pull-up by default)

	fd1 = fopen("/sys/class/gpio/export", "w");
	fprintf(fd1, "4");
	fclose(fd1);

	fd1 = fopen("/sys/class/gpio/gpio4/direction", "w");
	fprintf(fd1, "in");
	fclose(fd1);
 
	fd1 = fopen("/sys/class/gpio/gpio4/edge", "w");
	fprintf(fd1, "falling");
	fclose(fd1);
*/

/*

// Accessing the bus
fd_i2c = open(I2C_BUS, O_RDWR);
if (fd_i2c < 0) {
	perror(I2C_BUS);
	exit(EXIT_FAILURE);
	}

// setting fixed-cycle  timer clock source to 64Hz (TE = 0)
//rtc_write_reg(fd_i2c, RTC_ADDRESS,EXT,0x01);

// setting TC0 and TC1 for 1 pulse per 2 sec (64Hz / 128 = 0.5Hz)
//rtc_write_reg(fd_i2c, RTC_ADDRESS,TC0,128);
//rtc_write_reg(fd_i2c, RTC_ADDRESS,TC1,0);

// enabling interrup for TIE (CSEL1 = 0, CSEL0 = 1, TIE = 1)
//rtc_write_reg(fd_i2c, RTC_ADDRESS,CTL, 0x00000050);

// enabling interrup for AIE (CSEL1 = 0, CSEL0 = 0, TIE = 0)
rtc_write_reg(fd_i2c, RTC_ADDRESS,CTL, 0x08);

	// reset AIE
rtc_write_reg(fd_i2c, RTC_ADDRESS,FLAG,0x00);
	
// starting fixed-cycle timer (TE = 1)
//rtc_write_reg(fd_i2c, RTC_ADDRESS,EXT,0x11);
	
// fixed-cycle timer off
rtc_write_reg(fd_i2c, RTC_ADDRESS,EXT,0x00);	
	
// setting alarm to 20:11:00
rtc_write_reg(fd_i2c, RTC_ADDRESS,MIN_A,int2bcd(11));
rtc_write_reg(fd_i2c, RTC_ADDRESS,HOUR_A,int2bcd(20));
rtc_write_reg(fd_i2c, RTC_ADDRESS,WEEKDAY_A,0x80);
	
// setting time to 20:10:50
rtc_write_reg(fd_i2c, RTC_ADDRESS,SEC,int2bcd(50));
rtc_write_reg(fd_i2c, RTC_ADDRESS,MIN,int2bcd(10));
rtc_write_reg(fd_i2c, RTC_ADDRESS,HOUR,int2bcd(20));
*/
/*

	//Opening the "value" file
	if ((fd_select = open("/sys/class/gpio/gpio4/value", O_RDONLY)) < 0) {
		perror("/sys/class/gpio/gpio4/value");
		exit(EXIT_FAILURE);
	}
	
	int x=11;

	while (1) {
		FD_ZERO(& fds);
		FD_SET(fd_select, & fds);

		select(fd_select+1, NULL, NULL, & fds, NULL);
		// the process is stopped at this point and enters passive waiting.
		// It will resume when an interruption occurs and then....
		lseek(fd_select, 0, SEEK_SET); // rewinds the file
		read(fd_select, & buffer, 1); // dummy read, but it must be...



//fprintf(stdout,"time= %d:%d:%d (hh:mm:ss)\n",bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,HOUR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MIN)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,SEC)) );
	rtc_write_reg(fd_i2c, RTC_ADDRESS,MIN_A,int2bcd(x++));
		rtc_write_reg(fd_i2c, RTC_ADDRESS,SEC,int2bcd(50));
		rtc_write_reg(fd_i2c, RTC_ADDRESS,FLAG,0x00);

	} // end while


*/

	
// terminating
//	close(fd_i2c);
//	close(fd_select);
	return EXIT_SUCCESS;

}// end main





// converts  integer into BCD (only 8 bit taken into account)
int int2bcd(int data){

int units, tens;

tens = data / 10;
units = data % 10;

return (tens << 4) | units;
}

// converts  BCD into integer (only 8 bit taken into account)
int bcd2int(int data){

return ((data >> 4)* 10) +(data & 0x0000000F);
}

//reading temperature with default configuration: one-shot, 9-bit
float read_temperature(int fd, int chip_address, int reg_address){

unsigned char outbyte[2];
unsigned char inbyte[2];
int value;

	if (ioctl(fd, I2C_SLAVE,chip_address) < 0) {
	perror("slave unreachable");
	exit(EXIT_FAILURE);
	}

	outbyte[0] = (unsigned char)(reg_address);
	if (write(fd, & outbyte, 1) != 1){
	perror("write reg pointer");
	exit(EXIT_FAILURE);
	}

	if (read(fd, & inbyte, 2) != 2){
	perror("read TEMP");
	exit(EXIT_FAILURE);
	}


	value = (inbyte[0]<<1) + (inbyte[1]>>7);
	if (value>255) {
	value -= 511;
	}
	return value * 0.5;
}

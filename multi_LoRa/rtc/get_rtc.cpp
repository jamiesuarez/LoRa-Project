/*
getting RTC date and time
brs - /05/2016

*/



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
#define MINU		0x01
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




// prototypes
int rtc_write_reg(int, int, int, int);
int rtc_read_reg(int, int, int);
int int2bcd(int);
int bcd2int(int);


int main(int argc, char * argv[])
{

int fd_i2c;	// for I2C




// Accessing the bus
fd_i2c = open(I2C_BUS, O_RDWR);
if (fd_i2c < 0) {
	perror(I2C_BUS);
	exit(EXIT_FAILURE);
	}




fprintf(stdout,"time= %02d/%02d/20%02d %02d:%02d:%02d (dd/mm/yyyy hh:mm:ss) \n",bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,DAY)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MONTH)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,YEAR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,HOUR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MINU)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,SEC)));
	

	
// terminating
	close(fd_i2c);

	return EXIT_SUCCESS;

}// end main



// writes data at the given address
int rtc_write_reg(int fd, int chip_address, int reg_address, int data){

unsigned char outbyte[2];

if (ioctl(fd, I2C_SLAVE,chip_address) < 0) {
	perror("slave unreachable");
	exit(EXIT_FAILURE);
	}


outbyte[0] = (unsigned char)(reg_address);
outbyte[1] = (unsigned char)(data);

write(fd, & outbyte, 2);
return 0;
}


// reads data from the given address
int rtc_read_reg(int fd, int chip_address, int reg_address){

unsigned char outbyte[1];
unsigned char inbyte[1];

if (ioctl(fd, I2C_SLAVE,chip_address) < 0) {
	perror("slave unreachable");
	exit(EXIT_FAILURE);
	}



outbyte[0] = (unsigned char)(reg_address);
write(fd, & outbyte, 1);

read(fd, & inbyte, 1);

return inbyte[0];
}


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



/*
getting RTC date and time
brs - /05/2016

*/

/**************************************************************************************/

void get_date()
{

int fd_i2c;	// for I2C




// Accessing the bus
fd_i2c = open(I2C_BUS, O_RDWR);
if (fd_i2c < 0) {
	perror(I2C_BUS);
	exit(EXIT_FAILURE);
	}




fprintf(stdout,"%02d/%02d/20%02d %02d:%02d:%02d (dd/mm/yyyy hh:mm:ss) \n",bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,DAY)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MONTH)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,YEAR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,HOUR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MINU)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,SEC)));
	

	
// terminating
	close(fd_i2c);



}// end get_date()

/**************************************************************************************/

void get_date(FILE * fd)
{

int fd_i2c;	// for I2C




// Accessing the bus
fd_i2c = open(I2C_BUS, O_RDWR);
if (fd_i2c < 0) {
	perror(I2C_BUS);
	exit(EXIT_FAILURE);
	}




fprintf(fd,"%02d/%02d/20%02d %02d:%02d:%02d (dd/mm/yyyy hh:mm:ss) \n",bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,DAY)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MONTH)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,YEAR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,HOUR)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,MINU)),bcd2int(rtc_read_reg(fd_i2c, RTC_ADDRESS,SEC)));
	

	
// terminating
	close(fd_i2c);



}// end get_date(FILE *)

/**************************************************************************************/

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

/**************************************************************************************/

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

/**************************************************************************************/

// converts  integer into BCD (only 8 bit taken into account)
int int2bcd(int data){

int units, tens;

tens = data / 10;
units = data % 10;

return (tens << 4) | units;
}

/**************************************************************************************/

// converts  BCD into integer (only 8 bit taken into account)
int bcd2int(int data){

return ((data >> 4)* 10) +(data & 0x0000000F);
}


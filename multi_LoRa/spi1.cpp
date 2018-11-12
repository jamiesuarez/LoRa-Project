/* 
B.Stefanelli - nov 2016
Control of MCP4912
usage: mcp4912 <A|B> <value>
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <sys/ioctl.h>

#include <string.h>
#include <signal.h>


	char inbuf[10];
	char outbuf[10];
 
	struct spi_ioc_transfer xfer[1];

	char filename[40]="/dev/spidev0.1";

        int fd_spi;

	__u8    mode, lsb_first, bits;
    	__u32 speed;

 	int channel;
	int value;
	float vref, lsb;



// prototypes
int create_port(int);
int delete_port(int);
int set_port_direction(int, char *);
int set_port_value(int, int);
int get_port_value(int, int *);
int pulse_port(int, int);




int main(int argc, char *argv[])
{
// testing command integrity
        if (argc != 3) {
                fprintf(stderr, "usage: %s <A|B> <value>\n", argv[0]);
                exit(EXIT_FAILURE); 
        }

// getting the channel and testing it

	if (strcmp(argv[1],"A") == 0){
	channel = 0;
	} else if (strcmp(argv[1],"B") == 0) {
	channel = 1;
	} else {
        fprintf(stderr, "Channel must be A or B, found: %s\n", argv[1]);
        exit(EXIT_FAILURE);
        }

// getting the value to convert and testing it
        if (sscanf(argv[2], "%d", & value) != 1) {
                fprintf(stderr, "Wrong value: %s\n", argv[2]);
                exit(EXIT_FAILURE);
        }

	if(value > 1023){
	value = 1023;
	fprintf(stdout, "value clipped to 1023 \n");
	}
	if(value < 0){
 	value = 0;
	fprintf(stdout, "value clipped to 0 \n");
	}

	vref = 3.3;	// ref pin tied to VDD
	lsb=vref/1024;

// Opening the file
  
        fd_spi = open(filename, O_RDWR);
        if (fd_spi < 0) {
                perror(filename);
                exit(EXIT_FAILURE);
        }

// for _LDAC on GPIO24
/*	create_port(24);
	set_port_direction(24,"out");
	set_port_value(24,1);
*/
// setting mode : 0, 1, 2 or 3 (depends on the desired phase and/or polarity of the clock)
	mode = 0;
	ioctl(fd_spi, SPI_IOC_WR_MODE, &mode);

// getting some parameters
	if (ioctl(fd_spi, SPI_IOC_RD_MODE, &mode) < 0)
	{
         perror("SPI rd_mode");
         return -1;
         }
         if (ioctl(fd_spi, SPI_IOC_RD_LSB_FIRST, &lsb_first) < 0)
         {
         perror("SPI rd_lsb_fist");
         return -1;
         }
 

// setting maximum transfer speed, the effective speed will probably be different
	speed = 15000000;	// 15MHz asked

// setting bits per word
	bits = 8;
 
 	
 
	xfer[0].cs_change = 0; /* Keep CS activated if = 1 */
	xfer[0].delay_usecs = 0; //delay in us
	xfer[0].speed_hz = speed; //speed
	xfer[0].bits_per_word = bits; // bits per word 8


 
// clearing buffers
	memset(inbuf, 0, sizeof inbuf);
	memset(outbuf, 0, sizeof outbuf);

// filling buffers
// word for MCP4812/4912: _A/B 1 1 1 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0 x x
// BUF enabled, gain =1, no shutdown

	outbuf[0] = (channel << 7) | 0x70 | (value >> 6);	
	outbuf[1] = (value << 2);


	xfer[0].rx_buf = (unsigned long) inbuf;	// ignored here
	xfer[0].tx_buf = (unsigned long)outbuf;
	xfer[0].len = 2; /* Length of  command to write*/


// sending message

while(1){
 
	if(ioctl(fd_spi, SPI_IOC_MESSAGE(1), xfer) < 0) {
        perror("SPI_IOC_MESSAGE");
 //       exit(EXIT_FAILURE);
        }
}

// _LDAC pulse
//	pulse_port(24,0);	

// terminating the job
        close(fd_spi);

        return EXIT_SUCCESS;
}// end main



// creates the file corresponding to the given number in /sys/class/gpio
// arguments: portnumber = the port number
// returns: 0 = success, -1 = can't open /sys/class/gpio/export
// -2 = can't write to /sys/class/gpio/export
int create_port(int portnumber){
FILE *fd;


fd = fopen("/sys/class/gpio/export", "w");
if (fd == NULL){
	fprintf(stdout, "open export error");
	return -1;
}

if (fprintf(fd, "%d",  portnumber) < 0){
	fprintf(stdout, "write export error");
	return -2;
}

fclose(fd);
return 0;
}


// deletes the file corresponding to the given number in /sys/class/gpio
// arguments: portnumber = the port number
// returns: 0 = success, -1 = can't open /sys/class/gpio/unexport
// -2 = can't write to /sys/class/gpio/unexport
int delete_port(int portnumber){
FILE *fd;


fd = fopen("/sys/class/gpio/unexport", "w");
if (fd == NULL){
	fprintf(stdout, "open unexport error");
	return -1;
}

if (fprintf(fd, "%d", portnumber) < 0){
	fprintf(stdout, "write unexport error");
	return -2;
}

fclose(fd);
return 0;
}

// Determines the direction  of the port given 
// arguments: portnumlber = the port number, direction = in|out
// returns: 0 = success, -1 = can't open /sys/class/gpioXX/direction
// -2 = can't write to /sys/class/gpioXX/direction
int set_port_direction(int portnumber, char *direction){
FILE *fd;
char pnum[255]="";

char  filename[255]="/sys/class/gpio/gpio";

sprintf(pnum, "%d",portnumber);
strcat(filename, pnum);
strcat(filename,"/direction");


fd = fopen(filename, "w");
if (fd == NULL){
	fprintf(stdout, "open direction error");
	return -1;
}

if (fprintf(fd, "%s", direction) < 0){
	fprintf(stdout, "write direction error");
	return -2;
}

fclose(fd);

return 0;
}

// Sets the value  of the port given if configured as output
// arguments: portnumlber = the port number, value = 0|1
// returns: 0 = success, -1 = can't open /sys/class/gpioXX/value
// -2 = can't write to /sys/class/gpioXX/value
int set_port_value(int portnumber, int value){
FILE *fd;

char pnum[255]="";

char  filename[255]="/sys/class/gpio/gpio";

sprintf(pnum, "%d",portnumber);
strcat(filename, pnum);
strcat(filename,"/value");

fd = fopen(filename, "w");
if (fd == NULL){
	fprintf(stdout, "open value error");
	return -1;
}

if (fprintf(fd, "%d", value) < 0){
	fprintf(stdout, "write value error");
	return -2;
}

fclose(fd);
return 0;
}

// creates a pulse on the port given if configured as output
// arguments: portnumlber = the port number, pulsed_value = 0|1 (use set_port_value before to specify the quiescent level)
// returns: 0 = success, -1 = can't open /sys/class/gpioXX/value
// -2 = can't write to /sys/class/gpioXX/value
int pulse_port(int portnumber, int pulsed_value){
FILE *fd;

char pnum[255]="";

char  filename[255]="/sys/class/gpio/gpio";

sprintf(pnum, "%d",portnumber);
strcat(filename, pnum);
strcat(filename,"/value");

fd = fopen(filename, "w");
if (fd == NULL){
	fprintf(stdout, "open value error");
	return -1;
}

if (fprintf(fd, "%d", pulsed_value) < 0){
	fprintf(stdout, "write value error");
	return -2;
}

if (pulsed_value == 0)
	++pulsed_value;
else
	--pulsed_value;

rewind(fd);

if (fprintf(fd, "%d", pulsed_value) < 0){
	fprintf(stdout, "write value error");
	return -2;
}
fclose(fd);
return 0;
}


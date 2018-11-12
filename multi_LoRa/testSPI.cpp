#include "arduino2rpi/SPI.h"


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <sys/ioctl.h>


//struct spi_ioc_transfer xfer0[1];

SPI SPI;

int main(int argc, char **argv) {
	
	SPI.begin(SPI0);
//	SPI.begin(SPI1);
	
	int div;
	
	if (sscanf(argv[1], "%d", & div) != 1) exit(EXIT_FAILURE);
	
	SPI.setClockDivider(SPI0, div);
	
	SPI.transfer(SPI0, 0x05);
//	SPI.transfer(SPI1, 0x50);
	
/*
	char val[10];
	
	val[0]= 0x05;
	val[1]= 0x50;
	val[2]= 0x55;
*/
	
//	SPI.transfer(SPI0, val, 3);	
	
//	for (int i=0; i<3; i++){
//		fprintf(stdout, "val[%d] = 0x%02x\n", i, val[i]);
//	}



}
/*
BRS- 07/11/2018
generates a single ~10us pulse on  pin "pinName", then exits 
can be used to trigger an interrupt on the Rpi
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


int main(int argc, char **argv){
	
	FILE * fd;	
	char filename[255] ="";
	int pinNumber;
	
	// test command input
	if (argc != 2){
	fprintf(stdout, "usage: %s <pin number>\n", argv[0]);
	exit(EXIT_FAILURE);
	}// end if (argc != 2)
	
	if (sscanf(argv[1], "%d", & pinNumber) != 1) exit(EXIT_FAILURE);
	
	
	// continue if command OK
	
	sprintf(filename, "/sys/class/gpio/gpio%d",pinNumber);
	strcat(filename, "/value");
	
	// test if pin already exists, if not create it
	
	if ((fd = fopen(filename, "r+")) == NULL) {
	fd = fopen("/sys/class/gpio/export", "w");
	fprintf(fd, "%d", pinNumber);
	fclose(fd);
	} // end if
	
	sprintf(filename, "/sys/class/gpio/gpio%d",pinNumber);
	strcat(filename, "/direction");
	
	fd = fopen(filename, "w");
	fprintf(fd, "%s", "out");
	fclose(fd);

	sprintf(filename, "/sys/class/gpio/gpio%d",pinNumber);
	strcat(filename, "/value");
	fd = fopen(filename, "w");
	fprintf(fd, "%d", 0);
	rewind(fd);
	fprintf(fd, "%d", 1);
	rewind(fd);
	//usleep(10);
	fprintf(fd, "%d", 0);

	fclose(fd);
	
	return(EXIT_SUCCESS);
	
}

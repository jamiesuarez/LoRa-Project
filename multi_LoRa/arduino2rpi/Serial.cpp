#include "Serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

Serial::Serial(){}

void Serial::begin(int baudrate) {
fprintf(stdout,"Serial.begin(baudrate) forced to 115200\n");
}

void Serial::print(int val){
fprintf(stdout,"%d", val);
}

void Serial::print(int val, int format){
switch(format){
case 0:
fprintf(stdout,"%d", val);
break;

case 1:
fprintf(stdout,"%X", val);
break;

case 2:
fprintf(stdout,"%o", val);
break;

default:
fprintf(stdout,"%d", val);
break;
}
}

void Serial::println(int val, int format){

switch(format){
case 0:
fprintf(stdout,"%d\n", val);
break;

case 1:
fprintf(stdout,"%X\n", val);
break;

case 2:
fprintf(stdout,"%o\n", val);
break;

case 3:

	int binaryNum[1000];
	int i=0;
	int j;
	while (val > 0){
	binaryNum[i] = val %2;
	val = val/2;
	i++;
	}
	for (j=i-1; j>=0; j--){
	fprintf(stdout,"%d", binaryNum[j]);
	}
	fprintf(stdout,"\n");
break;


}

}



void Serial::println(int val){
fprintf(stdout,"%d\n", val);
}

void Serial::print(float val){
fprintf(stdout,"%-.2f", val);
}

void Serial::println(float val){
fprintf(stdout,"%-.2f\n", val);
}

void Serial::print(double val){
fprintf(stdout,"%-.2f", val);
}

void Serial::println(double val){
fprintf(stdout,"%-.2f\n", val);
}

void Serial::print(long val){
fprintf(stdout,"%ld", val);
}

void Serial::println(long val){
fprintf(stdout,"%ld\n", val);
}

void Serial::print(float val, int format){
fprintf(stdout,"%-.*f", format, val);
}

void Serial::println(float val, int format){
fprintf(stdout,"%-.*f\n", format, val);
}

void Serial::print(double val, int format){
fprintf(stdout,"%-.*f", format, val);
}

void Serial::println(double val, int format){
fprintf(stdout,"%-.*f\n", format, val);
}

void Serial::print(char val){
fprintf(stdout,"%c", val);
}

void Serial::println(char val){
fprintf(stdout,"%c\n", val);
}

void Serial::print(char* val){
fprintf(stdout,"%s", val);
}

void Serial::println(char* val){
fprintf(stdout,"%s\n", val);
}

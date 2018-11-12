#ifndef _UTILITIES_
#define _UTILITIES_

#include "../arduino2rpi/SPI.h"

SPI spi;


volatile unsigned * map_peripherals(void);
void pinMode(int, int);
void digitalWrite(int, int);
int digitalRead(int);
int createInterruptFile(int, int);
void calculateChannelSettings(int, int, int);
void  selectChannel(int);
__u8 readRegister(__u8);
void writeRegister(__u8, __u8);
void initModule(void);
void setChanHex(void);
void setLoraChannel(int);
__u32 getLoraChannel(void);
void printLoraChannelToStdout(__u32);
void printLoraChannelToFile(__u32, FILE *);

#endif

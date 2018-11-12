#ifndef RTC_H
#define RTC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <linux/i2c-dev.h>



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
void get_date(void);
//void get_date(int);
void get_date(FILE *);
int rtc_write_reg(int, int, int, int);
int rtc_read_reg(int, int, int);
int int2bcd(int);
int bcd2int(int);

#endif


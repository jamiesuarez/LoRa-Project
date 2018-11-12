/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#include "basics.h"

/******************************************************************************
 * Definitions & Declarations
 *****************************************************************************/
/*
volatile unsigned *gpio;
void *gpio_map;

// Rpi hardware access
//#define BCM2708_PERI_BASE        0x20000000	//RPi 1
#define BCM2708_PERI_BASE	 0x3F000000	//RPi 2 & 3
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) // GPIO controller 
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GPIO_SET_EXT *(gpio+8)  // for GPIO# >= 32 RPi B+, A+, 2, 3
#define GPIO_CLR_EXT *(gpio+11) // for GPIO# >= 32 RPi B+, A+, 2, 3
 
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
#define GET_GPIO_EXT(g) (*(gpio+14)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
#define GPIO_PULLCLK1 *(gpio+39) // Pull up/pull down clock for GPIO# >= 32 RPi B+, A+, 2, 3

*/
 
/******************************************************************************
 * Function prototypes
 *****************************************************************************/

/******************  digital I/O  ********************/

/*
defines pin mode
pin = gpio pin, mode = 0|1|2
returns none
*/
void pinMode(int pin, int mode){

switch(mode){

	case 0:
	INP_GPIO(pin); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(pin);
	break;

	case 1:
	INP_GPIO(pin);
	GPIO_PULL = 0;
	break;

	case 2:
	INP_GPIO(pin);
	GPIO_PULL = 2;
	break;

	default:
	INP_GPIO(pin);
	GPIO_PULL = 0;
	break;

}

// sets pull-up/float
usleep(10);

if (pin>= 32)
GPIO_PULLCLK1 = (1 << (pin % 32));
else
GPIO_PULLCLK0 = (1 << pin);

usleep(10);

GPIO_PULL = 0;

if (pin>= 32)
GPIO_PULLCLK1 = 0;
else
GPIO_PULLCLK0 = 0;

}// end pinMode(int pin, int mode)

/*
defines pin value
pin = gpio pin, value = 0|1
returns none
*/
void digitalWrite(int pin, int value){

switch(value){

	case 0:
	if (pin>= 32)
	GPIO_CLR_EXT = (1 << (pin % 32));
	else
	GPIO_CLR = (1 << pin);
	break;

	case 1:
	if (pin>= 32)
	GPIO_SET_EXT = (1 << (pin % 32));
	else
	GPIO_SET = (1 << pin);
	break;

	default:
	// no change
	break;
}

} // end void digitalWrite(int pin, int mode)


/*
gets pin value
pin = gpio pin
returns 0|1
*/
int digitalRead(int pin){

  if (GET_GPIO(pin)) // !=0 <-> bit is 1 <- port is HIGH=3.3V
    return 1;
  else // port is LOW=0V
    return 0;

} // end void digitalRead(int pin)


/******************  end digital I/O  ********************/


/******************  Time  ********************/

/*

NOTE: delay(ms) and delayMicroseconds(us) implemented as macros

*/

/******************  end Time  ********************/

/******************  Serial  ********************/

/******************  end Serial  ********************/


/***************** non-arduino function *****************/

/*
Set up a memory regions to access peripherals
argument(s) none
returns none
*/

void map_peripherals(void)
{
int  mem_fd;

   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      perror("opening /dev/mem");
      exit(-1);
   }
 
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
 
   
 
   if (gpio_map == MAP_FAILED) {
      perror("mmap(/dev/mem)");//errno also set!
      exit(-1);
   }
 
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;

 
close(mem_fd); //No need to keep mem_fd open after mmap
 
 
} // end map_peripherals



/***************** end non-arduino function *****************/


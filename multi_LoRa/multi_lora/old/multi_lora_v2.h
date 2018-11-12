#ifndef _MULTI_LORA_V2_
#define _MULTI_LORA_V2_




// MCP7490
//#define	RTC_ADDRESS	0x6f
// RX8803
#define	RTC_ADDRESS	0x32
 

#define RXCOUNT 4

#define RXDONE0_INT 6
#define RXDONE1_INT 12
#define RXDONE2_INT 13
#define RXDONE3_INT 19
#define TXDONE_INT 27
#define RESETPIN 16
#define RTCALARM 4
#define SEL0 24
#define SEL1 23
#define SEL2 22
#define RX_CSPIN 26


#define HIGH 1
#define LOW 0
#define INPUT 1
#define OUTPUT 0
#define INPUT_PULLUP 2

#define FALLING 0
#define RISING 1

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




#endif

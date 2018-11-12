// arduino-style programming on Rpi
 

#include "arduino2rpi/basics.h"
#include "arduino2rpi/Serial.h"
#include "arduino2rpi/SPI.h"

//#define MSBFIRST 0


//**************************************************************************************
//********************* function prototypes ********************************************
//**************************************************************************************




//**************************************************************************************
//******************** main ************************************************************
//**************************************************************************************

char toto;

// added for compatibility 
int main(int argc, char **argv){

Serial Serial;
SPI SPIx;
 
// Set up pointers for direct register access
map_peripherals();

// end added for compatibility 


// Set up IO ports 

pinMode(24, OUTPUT);

Serial.begin(9600);


int a=83;
int b=456;
float c=123.456;
double d=123456789.987654321;
/*
Serial.print(a);
Serial.println(b);
Serial.println(c);
Serial.println(d);
Serial.println(c, 5);
Serial.println(d, 6);
Serial.println("toto");
*/
Serial.println(a);
Serial.println(char(a));
Serial.println((char)a);

Serial.println(a, DEC);
Serial.println(a, HEX);
Serial.println(a, OCT);
Serial.println(a, BIN);


/*

while (1){


digitalWrite(24, LOW);
delay(200);
digitalWrite(24, HIGH);
delayMicroseconds(200000);

}//end while

*/

/*Serial.println("test SPI");

  SPIx.begin();
  SPIx.setBitOrder(MSBFIRST);
  SPIx.setClockDivider(21);
  SPIx.setDataMode(SPI_MODE0);

toto = SPIx.transfer(0x55);
*/

  return 0;
 
} // end  main
 
 
//**************************************************************************************
//******************** subroutines *****************************************************
//**************************************************************************************




/* ***************************************************
  SX1272 en mode LoRa
  RXCONTINUOUS
**************************************************** */

/* added or modified for arduino to rpi  */
//#include <Wire.h>
//#include <SPI.h>
#include "arduino2rpi/basics.h"
#include "arduino2rpi/Serial.h"
#include "arduino2rpi/SPI.h"

//#include "SX1272_registers.h"
#include "SX1272/SX1272_registers.h"
/* end added or modified for arduino to rpi  */

#include <sys/select.h>
#include <sys/time.h>


/* added or modified for arduino to rpi  */
//#define CSPIN 3
//#define RXPIN 4
//#define TXPIN 5
//#define RESETPIN 6
#define CSPIN 21
#define RXPIN 20
#define TXPIN 26
#define RESETPIN 16
/* end added or modified for arduino to rpi  */


//byte replaced by __u8
//uint8_t replaced by __u8

__u8 reg_val;
int i, rssi;

	int fd;
	fd_set fds;
	struct timeval tv;
	char buffer[2];


/* added for arduino to rpi  */

void setup(void);
void loop(void);
void antennaTX();
void antennaRX();
void writeRegister(__u8, __u8);
__u8 readRegister(__u8);
void variableHex(char *, __u8);
void variableDec(char * , __u8);
void variable(char *, int);
void initModule(void);
void getMode(void);



Serial Serial;
SPI SPI;

int main(int argc, char **argv){




	//Open the value file 
	if ((fd = open("/sys/class/gpio/gpio12/value", O_RDONLY)) < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}



 
map_peripherals();

setup();

while(1) {
loop();
}

close(fd);
return 0;

} // end main

/* end added for arduino to rpi  */





void setup() {
  // init des pins
  pinMode(TXPIN, OUTPUT);
  digitalWrite(TXPIN, LOW);
  pinMode(RXPIN, OUTPUT);
  digitalWrite(RXPIN, LOW);
  pinMode(RESETPIN, INPUT);


  // init console
  Serial.begin(115200);
  Serial.println("****SETUP****");

  // init SPI
  SPI.begin();
  pinMode(CSPIN, OUTPUT);
  digitalWrite(CSPIN, HIGH);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);

  // reset du module
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH);
  delay(50);
  pinMode(RESETPIN, INPUT);
  delay(50);

  // module en TX
  antennaTX();

  // module en mode lora
  writeRegister(REG_OP_MODE, FSK_SLEEP_MODE); // pour pouvoir changer le bit 7!!
  writeRegister(REG_OP_MODE, LORA_SLEEP_MODE);
  getMode();
  // initialise le module (TX et RX)
  initModule();

  // interruptions configuration
  // initModule() unmasks ALL interruptions by default
  writeRegister(REG_DIO_MAPPING1, 0x01); // DIO0=RxDone, DIO1=RxTimeout, DIO2=FhssChangeChannel, DIO3=ValidHeader
  writeRegister(REG_DIO_MAPPING2, 0x00); // default values



}// end setup

void loop() {

  Serial.println("****RXCONTINUOUS mode****");

  // module en RX
  antennaRX();

  // step 1: FifoRxBaseDddress -> FifoAddrPtr
  writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_BASE_ADDR));

  //Step2: done in init

  // Step3: RX continuous mode
  writeRegister(REG_OP_MODE, LORA_RX_CONTINUOUS_MODE);
  delay(100); // il faut ce delai  pour démarrer l'oscillateur et la PLL (SLEEP -> autre mode)
  getMode();


  while (true)
  {

    //Step4: Checking reception of a packet

	// Resets the table of the events we are waiting for
	FD_ZERO(& fds);
	// adds the file descriptor previously obtained
	FD_SET(fd, & fds);

      Serial.print("loop2: waiting for rxDone IRQ... ");
	// passive waiting (no timeout, therefore infinite waiting...
	if (select(fd+1, NULL, NULL, & fds, NULL) < 0) {
		perror("select");
		break;
	}

      Serial.print(" rxDone IRQ received at: ");

	// reads the time at which the event occured
	gettimeofday(& tv, NULL);

	Serial.print(tv.tv_sec);
	Serial.print(".");
	Serial.println(tv.tv_usec);


		// rewinds the file (necessary for multiple readings).
		lseek(fd, 0, SEEK_SET);
		//reads GPIO value (2 bytes).
		if (read(fd, & buffer, 2) != 2) {
			perror("read");
			break;
		}

    // Step 5: test CRC
    if (bitRead(reg_val, 5) == 0x01) Serial.println("loop8: CRC error");

    // step 6: récup data
    writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR)); //REG_FIFO_RX_CURRENT_ADDR -> FifoAddrPtr
    for (i = 0; i < (int)readRegister(REG_RX_NB_BYTES); i++)
    {
      Serial.print("i= ");
      Serial.print(i, DEC);
      Serial.print("  data= ");
      //      Serial.println(readRegister(REG_FIFO), HEX);
      reg_val = readRegister(REG_FIFO);
      Serial.print(reg_val, HEX);
      Serial.print(", ");
     Serial.println((char)reg_val);

    }


    getMode();

    variableHex("loop5b: REG_IRQ_FLAGS = 0x", readRegister(REG_IRQ_FLAGS));
    writeRegister(REG_IRQ_FLAGS, 0xff);  // RAZ de toutes IRQ
    variableHex("loop5c: REG_IRQ_FLAGS = 0x", readRegister(REG_IRQ_FLAGS));  // teste INT

    variableDec("loop6: REG_PAYLOAD_LENGTH_LORA = ", readRegister(REG_PAYLOAD_LENGTH_LORA));  // taille du paquet reçu
    variableHex("loop7: REG_FIFO_RX_CURRENT_ADDR = 0x", readRegister(REG_FIFO_RX_CURRENT_ADDR));
    variableDec("loop7: REG_RX_NB_BYTES = ", readRegister(REG_RX_NB_BYTES));

    reg_val = readRegister(REG_RSSI_VALUE_LORA);
    variableDec("loop8: REG_RSSI_VALUE_LORA = ", reg_val);
    rssi = -139 + reg_val;
    variable("loop8: RSSI (dBm) = ", rssi);


  } // end while(true)



} // end loop


//***********************************************************
// *********** Fonctions ************************************

void antennaTX()
{
  digitalWrite(TXPIN, LOW);
  digitalWrite(RXPIN, LOW);
  delay(10);
  digitalWrite(TXPIN, HIGH);
}

void antennaRX()
{
  digitalWrite(TXPIN, LOW);
  digitalWrite(RXPIN, LOW);
  delay(10);
  digitalWrite(RXPIN, HIGH);
}

void writeRegister(__u8 address, __u8 data)
{
  digitalWrite(CSPIN, LOW);
  bitSet(address, 7);     // Bit 7 set to read from registers
  SPI.transfer(address);
  SPI.transfer(data);
  digitalWrite(CSPIN, HIGH);

}

__u8 readRegister(__u8 address)
{
  __u8 value = 0x00;

  digitalWrite(CSPIN, LOW);

  bitClear(address, 7);   // Bit 7 cleared to write in registers
  // BRS: pas cohérent avec datasheet p76
  // mais la datasheet semble erronée car elle fait vréférence
  // "nwr", donc bien 0 pour écrire
  SPI.transfer(address);
  value = SPI.transfer(0x00);

  digitalWrite(CSPIN, HIGH);

  return value;
}

void variableHex(char * texte, __u8 variable)
{
  Serial.print(texte);
  Serial.println(variable, HEX);
}

void variableDec(char * texte, __u8 variable)
{
  Serial.print(texte);
  Serial.println(variable, DEC);
}

void variable(char * texte, int variable)
{
  Serial.print(texte);
  Serial.println(variable);
}

void initModule(void)
{
  writeRegister(REG_FIFO, 0x00);

  writeRegister(REG_FRF_MSB, 0xd8); // center freq = 865.2MHz
  writeRegister(REG_FRF_MID, 0x4c);
  writeRegister(REG_FRF_LSB, 0xcc);

  writeRegister(REG_PA_CONFIG, 0x01); // out=RFIO, Pout=0dBm

  writeRegister(REG_PA_RAMP, 0x19); // low cons PLL TX&RX, 40us

  writeRegister(REG_OCP, 0b00101011); //OCP enabled, 100mA

  writeRegister(REG_LNA, 0b00100011); // max gain, BOOST on

  writeRegister(REG_FIFO_ADDR_PTR, 0x00); // pointeur pour l'accès à la FIFO via SPI (read or write)
  writeRegister(REG_FIFO_TX_BASE_ADDR, 0x80); // top half
  writeRegister(REG_FIFO_RX_BASE_ADDR, 0x00); // bottom half

  writeRegister(REG_IRQ_FLAGS_MASK, 0x00);  // toutes IRQ actives

  writeRegister(REG_IRQ_FLAGS, 0xff);  // RAZ de toutes IRQ

  // en mode Explicit Header, CRC enable ou disable n'a aucune importance pour RX, tout dépend de la config TX
  writeRegister(REG_MODEM_CONFIG1, 0b10001000); //BW=500k, CR=4/5, explicit header, CRC disable, LDRO disabled
  //writeRegister(REG_MODEM_CONFIG1, 0b10001010); //BW=500k, CR=4/5, explicit header, CRC enable, LDRO disabled

  writeRegister(REG_MODEM_CONFIG2, 0b11000111); // SF=12, normal TX mode, AGC auto on, RX timeout MSB = 11

  writeRegister(REG_SYMB_TIMEOUT_LSB, 0xff);  // max timeout

  writeRegister(REG_PREAMBLE_MSB_LORA, 0x00); // default value
  writeRegister(REG_PREAMBLE_LSB_LORA, 0x08);

  writeRegister(REG_MAX_PAYLOAD_LENGTH, 0x80); // half the FIFO

  writeRegister(REG_HOP_PERIOD, 0x00); // freq hopping disabled

  writeRegister(REG_DETECT_OPTIMIZE, 0xc3); // pour SF=12

  writeRegister(REG_INVERT_IQ, 0x27); // default value, IQ not inverted

  writeRegister(REG_DETECTION_THRESHOLD, 0x0A); // pour SF=12

  writeRegister(REG_SYNC_WORD, 0x12);   // default value
}

void getMode(void)
{
  __u8 reg;
  reg = readRegister(REG_OP_MODE);
  if (bitRead(reg, 7) == 1)
  {
    if (bitRead(reg, 6) == 0) Serial.print("-> LoRa ");
    else Serial.print("-> LoRa with FSK regs access ");

    reg = reg & 0x7;
    switch (reg) {
      case 0:
        Serial.println("sleep mode");
        break;
      case 1:
        Serial.println("standby mode");
        break;
      case 2:
        Serial.println("FSTx mode");
        break;
      case 3:
        Serial.println("Tx mode");
        break;
      case 4:
        Serial.println("FSRx mode");
        break;
      case 5:
        Serial.println("Rx continuous mode");
        break;
      case 6:
        Serial.println("Rx single mode");
        break;
      case 7:
        Serial.println("CAD mode");
        break;
    }
  }
  else {
    Serial.print("-> FSK ");

    reg = reg & 0x7;
    switch (reg) {
      case 0:
        Serial.println("sleep mode");
        break;
      case 1:
        Serial.println("standby mode");
        break;
      case 2:
        Serial.println("FSTx mode");
        break;
      case 3:
        Serial.println("Tx mode");
        break;
      case 4:
        Serial.println("FSRx mode");
        break;
      case 5:
        Serial.println("Rx mode");
        break;
      default:
        Serial.println("error: reserved mode");
        break;
    }
  }


}

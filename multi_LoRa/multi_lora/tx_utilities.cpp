

/****************************************************************************/
volatile unsigned * map_peripherals(void)
{
void *gpio_map;
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
 //  gpio = (volatile unsigned *)gpio_map;
	return (volatile unsigned *)gpio_map;
 
close(mem_fd); //No need to keep mem_fd open after mmap
 
 
} // end map_peripherals

/****************************************************************************/
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

/****************************************************************************/
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

/****************************************************************************/
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

/****************************************************************************/
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


/****************************************************************************/
int createInterruptFile(int pin, int edge){

int fdreturned;
char filename[255] ="";
char pinval[3]="";
FILE * fd;

sprintf(filename, "/sys/class/gpio/gpio%d",pin);
strcat(filename, "/edge");

if ((fd = fopen(filename, "r+")) == NULL) {
	fd = fopen("/sys/class/gpio/export", "w");
	sprintf(pinval, "%d",pin);
	fprintf(fd, "%s", pinval);
	fclose(fd);
	sprintf(filename, "/sys/class/gpio/gpio%d", pin);
	strcat(filename, "/edge");
	fd = fopen(filename, "r+");

	switch(edge){
	case 0:	
	fprintf(fd, "falling");
	break;
	case 1:	
	fprintf(fd, "rising");
	break;
	default:
	fprintf(fd, "falling");
	break;
	}

	fclose(fd);

} else {

	switch(edge){
	case 0:	
	fprintf(fd, "falling");
	break;
	case 1:	
	fprintf(fd, "rising");
	break;
	default:
	fprintf(fd, "falling");
	break;
	}

fclose(fd);
}

sprintf(filename, "/sys/class/gpio/gpio%d", pin);
strcat(filename, "/value");
if ((fdreturned = open(filename, O_RDONLY)) < 0) {
	perror("value file: ");
	return -1;
	exit(EXIT_FAILURE);
} else {
	return fdreturned;
}

}// end int createInterruptFile(int, int);

/****************************************************************************/
void calculateChannelSettings(int sel0, int sel1, int sel2){

channelClear = (__u32)(pow(2,(double)sel2) + pow(2,(double)sel1) + pow(2,(double)sel0));


channelSet[0] =	0;
channelSet[1] = (__u32)(pow(2,(double)sel0));
channelSet[2] = (__u32)(pow(2,(double)sel1));
channelSet[3] = (__u32)(pow(2,(double)sel1) + pow(2,(double)sel0));
channelSet[4] = (__u32)(pow(2,(double)sel2));
channelSet[5] = (__u32)(pow(2,(double)sel2) + pow(2,(double)sel0));
channelSet[6] = (__u32)(pow(2,(double)sel2) + pow(2,(double)sel1));
channelSet[7] = (__u32)(pow(2,(double)sel2) + pow(2,(double)sel1) + pow(2,(double)sel0));


} // end calculateChannelSettings(int, int, int, __u32)

/****************************************************************************/
void  selectChannel(int channel){

GPIO_CLR = channelClear;

if((channel >= 0) || (channel <= 7)){
	GPIO_SET = channelSet[channel];
}

} // end  selectChannel(int);

/****************************************************************************/

void writeRegister(__u8 address, __u8 data)
{

bitSet(address, 7);     // Bit 7 set to write in registers
	
	char buffer[2];
	buffer[0]= address;
	buffer[1]= data;

	spi.transfer(SPI0, buffer, 2);

	
}// end writeRegister()


/****************************************************************************/

__u8 readRegister(__u8 address)
{

bitClear(address, 7);   // Bit 7 cleared to read from registers
	
	char buffer[2];
	buffer[0]= (char) address;
	buffer[1]= (char) 0x55;

	spi.transfer(SPI0, buffer, 2);
	return (__u8) buffer[1];
	

}// end readRegister()

/****************************************************************************/

void initModule(void)
{
  writeRegister(REG_FIFO, 0x00);

//  writeRegister(REG_PA_CONFIG, 0x01); // out=RFIO, Pout=0dBm
// out=PA_BOOST, Pout=0dBm
	int pout = 0;
  writeRegister(REG_PA_CONFIG, 0x80 | pout); 

  writeRegister(REG_PA_RAMP, 0x19); // low cons PLL TX&RX, 40us

  writeRegister(REG_OCP, 0b00101011); //OCP enabled, 100mA

  writeRegister(REG_LNA, 0b00100011); // max gain, BOOST on

  writeRegister(REG_FIFO_ADDR_PTR, 0x00); // pointeur pour l'accès à la FIFO via SPI (read or write)
  writeRegister(REG_FIFO_TX_BASE_ADDR, 0x80); // top half
  writeRegister(REG_FIFO_RX_BASE_ADDR, 0x00); // bottom half

  writeRegister(REG_IRQ_FLAGS_MASK, 0x00);  // toutes IRQ actives

  writeRegister(REG_IRQ_FLAGS, 0xff);  // RAZ de toutes IRQ

  // en mode Explicit Header, CRC enable ou disable n'a aucune importance pour RX, tout dépend de la config TX
  //writeRegister(REG_MODEM_CONFIG1, 0b10001000); //BW=500k, CR=4/5, explicit header, CRC disable, LDRO disabled
  //writeRegister(REG_MODEM_CONFIG1, 0b00001001); //BW=125k, CR=4/5, explicit header, CRC disable, LDRO enabled
  //writeRegister(REG_MODEM_CONFIG1, 0b00100001); //BW=125k, CR=4/8, explicit header, CRC disable, LDRO enabled
  writeRegister(REG_MODEM_CONFIG1, 0b00100011); //BW=125k, CR=4/8, explicit header, CRC enable, LDRO enabled
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

/****************************************************************************/

void setChanHex(void){

chanHex[0] = 0xD84CCC; // channel 10, central freq = 865.20MHz
chanHex[1] = 0xD86000; // channel 11, central freq = 865.50MHz
chanHex[2] = 0xD87333; // channel 12, central freq = 865.80MHz
chanHex[3] = 0xD88666; // channel 13, central freq = 866.10MHz
chanHex[4] = 0xD89999; // channel 14, central freq = 866.40MHz
chanHex[5] = 0xD8ACCC; // channel 15, central freq = 866.70MHz
chanHex[6] = 0xD8C000; // channel 16, central freq = 867.00MHz
chanHex[7] = 0xD90000; // channel 16, central freq = 868.00MHz

}

/****************************************************************************/

void setLoraChannel(int channel){

  writeRegister(REG_FRF_LSB, chanHex[channel] & 0x000000ff);

  writeRegister(REG_FRF_MID, (chanHex[channel] >> 8) & 0x000000ff);
  
  writeRegister(REG_FRF_MSB, (chanHex[channel] >> 16) & 0x000000ff);
}

/****************************************************************************/

__u32 getLoraChannel(void){
  __u32 channel;

  channel = readRegister(REG_FRF_MSB)*65536 + readRegister(REG_FRF_MID)*256 + readRegister(REG_FRF_LSB);
  return channel;
}

/****************************************************************************/

void printLoraChannelToStdout(__u32 channel){
switch(channel){
case 0xD84CCC:
fprintf(stdout,"channel 10, central freq = 865.20MHz");
break;
case 0xD86000:
fprintf(stdout,"channel 11, central freq = 865.50MHz");
break;
case 0xD87333:
fprintf(stdout,"channel 12, central freq = 865.80MHz");
break;
case 0xD88666:
fprintf(stdout,"channel 13, central freq = 866.10MHz");
break;
case 0xD89999:
fprintf(stdout,"channel 14, central freq = 866.40MHz");
break;
case 0xD8ACCC:
fprintf(stdout,"channel 15, central freq = 866.70MHz");
break;
case 0xD8C000:
fprintf(stdout,"channel 16, central freq = 867.00MHz");
break;
case 0xD90000:
fprintf(stdout,"channel 17, central freq = 868.00MHz");
break;
default:
fprintf(stdout,"!ERROR! unknown channel");
break;
}

}

/****************************************************************************/

void printLoraChannelToFile(__u32 channel, FILE * fd){
switch(channel){
case 0xD84CCC:
fprintf(fd,"channel 10, central freq = 865.20MHz");
break;
case 0xD86000:
fprintf(fd,"channel 11, central freq = 865.50MHz");
break;
case 0xD87333:
fprintf(fd,"channel 12, central freq = 865.80MHz");
break;
case 0xD88666:
fprintf(fd,"channel 13, central freq = 866.10MHz");
break;
case 0xD89999:
fprintf(fd,"channel 14, central freq = 866.40MHz");
break;
case 0xD8ACCC:
fprintf(fd,"channel 15, central freq = 866.70MHz");
break;
case 0xD8C000:
fprintf(fd,"channel 16, central freq = 867.00MHz");
break;
case 0xD90000:
fprintf(fd,"channel 17, central freq = 868.00MHz");
break;
default:
fprintf(fd,"!ERROR! unknown channel");
break;
}

}
















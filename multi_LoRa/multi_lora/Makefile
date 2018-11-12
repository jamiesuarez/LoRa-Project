
CC=arm-linux-gcc

CPP=arm-linux-g++

TARGET?=

WARN= -Wall

LIBPATH=/home/tom/libraries

LIB= -lm

#ARDUINO_LIBS= basics.o Serial.o SPI.o rtc.o utilities.o MCP7940.o
ARDUINO_LIBS?= 

$(TARGET) : $(TARGET).o $(ARDUINO_LIBS)
	$(CPP) $(TARGET).o $(ARDUINO_LIBS) -o $(TARGET)

$(TARGET).o : $(TARGET).cpp
	$(CPP) -c $(LIB) $(WARN) $(TARGET).cpp

basics.o: $(LIBPATH)/arduino2rpi/basics.cpp
	$(CPP) -c $(LIB) $(WARN) $(LIBPATH)/arduino2rpi/basics.cpp

Serial.o: $(LIBPATH)/arduino2rpi/Serial.cpp
	$(CPP) -c $(LIB) $(WARN) $(LIBPATH)/arduino2rpi/Serial.cpp

SPI.o: $(LIBPATH)/arduino2rpi/SPI.cpp
	$(CPP) -c $(LIB) $(WARN) $(LIBPATH)/arduino2rpi/SPI.cpp

rtc.o: $(LIBPATH)/arduino2rpi/rtc.cpp
	$(CPP) -c $(LIB) $(WARN) $(LIBPATH)/arduino2rpi/rtc.cpp

MCP7940.o: $(LIBPATH)/arduino2rpi/MCP7940.cpp
	$(CPP) -c $(LIB) $(WARN) $(LIBPATH)/arduino2rpi/MCP7940.cpp

rx_utilities.o: ./multi_lora/rx_utilities.cpp
	$(CPP) -c $(LIB) $(WARN) ./multi_lora/rx-utilities.cpp
	
tx_utilities.o: ./multi_lora/tx_utilities.cpp
	$(CPP) -c $(LIB) $(WARN) ./multi_lora/tx_utilities.cpp
	
utilities.o: ./multi_lora/utilities.cpp
	$(CPP) -c $(LIB) $(WARN) ./multi_lora/utilities.cpp
		 
clean:
	-rm *.o


CC=arm-linux-gcc

CPP=arm-linux-g++

TARGET?=

WARN= -Wall

LIB= -lm

ARDUINO_LIBS?=

#ARDUINO_LIBS= basics.o Serial.o SPI.o rtc.o utilities.o


$(TARGET) : $(TARGET).o $(ARDUINO_LIBS)
	$(CPP) $(TARGET).o $(ARDUINO_LIBS) -o $(TARGET)

$(TARGET).o : $(TARGET).cpp
	$(CPP) -c $(LIB) $(WARN) $(TARGET).cpp

basics.o: ./arduino2rpi/basics.cpp
	$(CPP) -c $(LIB) $(WARN) ./arduino2rpi/basics.cpp

Serial.o: ./arduino2rpi/Serial.cpp
	$(CPP) -c $(LIB) $(WARN) ./arduino2rpi/Serial.cpp

SPI.o: ./arduino2rpi/SPI.cpp
	$(CPP) -c $(LIB) $(WARN) ./arduino2rpi/SPI.cpp

rtc.o: ./arduino2rpi/rtc.cpp
	$(CPP) -c $(LIB) $(WARN) ./arduino2rpi/rtc.cpp

utilities.o: ./multi_lora/utilities.cpp
	$(CPP) -c $(LIB) $(WARN) ./multi_lora/utilities.cpp
		 
clean:
	-rm *.o

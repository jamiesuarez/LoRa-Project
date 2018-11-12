emitter (TX) and receiver (Rx):
3 accounts on the RPi platform
login: lora / root / www
passwd: lora / root / www



software:

Now, Tx and Rx have default parameters (see the web page on the server for details)

on the console termibal: the ifconfig gives the IP address (normally 192.168.2.10 for Rx and 192.168.2.15 for Tx)

RTC: (real-time clock)

./set_rtc: to set the current date 
./get_rtc: to display the current date 

RX:

./multi_lora_rx_v1d <channel number> <timeout us> <backup hour> <backup min>

	1/ the file-system must be read-write (rw command) for proper operation
	2/ multi_lora_rx_v1d must be launched in background 
	3/ in a web browser, type the IP address to open the interface
	4/ backups on the SD card  in the  "backups" directory under /home/lora

working files:
	virtual files under /tmp (new files created each time you run the software)
	channelxx.index = current index of messages for channel xx (index of the last received message, 0 if no message)
	channelxx.log = contents of all the received messages since the last backup(example below)

000000000: No message received yet!
000000001: Valid header received on channel 10, central freq = 865.20MHz
Time stamp: 10/10/2018 10:04:14 (dd/mm/yyyy hh:mm:ss) 
Message received
Payload CRC OK (INFO: test relevance depends on transmitter configuration)
Number of bytes received = 9
byte 0 data= 0x74 , t
byte 1 data= 0x65 , e
byte 2 data= 0x73 , s
byte 3 data= 0x74 , t
byte 4 data= 0x5F , _
byte 5 data= 0x63 , c
byte 6 data= 0x68 , h
byte 7 data= 0x31 , 1
byte 8 data= 0x30 , 0
Message as string:test_ch10
RSSI (dBm) = -103

Program architecture:
main: multi_lora_rx_v1d

library calls: rx_utilities, SPI, SX1272_registers, rtc_rx8803




TX:

./multi_lora_tx_v1 <channel number from 10 to 17> <message (128 characters max)>
	1/ keep the transmitter at least 1m aprt from the receivers (if not, saturation of the receivers which needs a resart of the program. This is an unresolved issue not in the scope of the project)
	2/ in a web browser, type the IP address to open the interface







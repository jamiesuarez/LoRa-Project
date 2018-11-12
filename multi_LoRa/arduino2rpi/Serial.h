#ifndef SERIAL_H
#define SERIAL_H

/******************************************************************************
 * Includes
 ******************************************************************************/

/******************************************************************************
 * Definitions & Declarations
 *****************************************************************************/
#define DEC 0
#define HEX 1
#define OCT 2
#define BIN 3
/******************************************************************************
 * Function prototypes & macros
 *****************************************************************************/

class Serial {

public:
	Serial();
	void begin(int); //baudrate definition not supported, defaults to 115200

	void print(int);
	void println(int);

	void print(int, int);
	void println(int, int);

	void print(float);
	void println(float);

	void print(double);
	void println(double);

	void print(long);
	void println(long);

	void print(float, int);
	void println(float, int);

	void print(double, int);
	void println(double, int);

	void print(char);
	void println(char);

	void print(char*);
	void println(char*);

};

#endif

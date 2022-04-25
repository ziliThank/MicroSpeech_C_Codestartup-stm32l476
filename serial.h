// serial.h
// Functions available in the stm32 serial library
// Author: Frank Duignan
#define NEWLINE 0x0d
#define LINEFEED 0x0a

void initUART(int BaudRate);
int ReadCom(int Max,unsigned char *Buffer);
int WriteCom(int Count,unsigned char *Buffer);
int eputs(char *String);
int egets(char *String, int size);
int available();
void eputLong(unsigned long Number);
int available(void);
void printHex(unsigned int Number);
void drainTX(void);
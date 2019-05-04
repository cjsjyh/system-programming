#ifndef _UTIL_H_
#define _UTIL_H_

//Checking Functions
int checkRange(int);
int compareString(char*, char*, char*);
int isDec(char*);
int isHex(char*);
int inRange(int,int,int);
int HexBitCount(int);
int checkComma(char*);
int checkHex(char*);

//Parsing
int asmSeparater(char*, char*, char*, char*, char*);
char* extractContent(char*);
void extractStr(char* dest, char* source, int start, int len);
int extractStrToHex(char* source, int start, int len);

//Data manipulation
void LowerToUpper(char*);
int StrToInt(char*);
int StrToHex(char*);
int insertHexAt(int,int,int);
void charArrHexCal(unsigned char* arr, int value, int len, char operation);

//BIT functions
int bitFormat4(int addr);
int bitToHex(int addr,int start,int end);
int bitAddressMode(int addr);

//Register functions
int registerNum(char*);
void compareReg(int a, int b);
void printReg();
void writeToMem(int addr, int regNum);
int getLastByte(int regNum);
void storeLastByte(int regNum, int value);

#endif
#ifndef _UTIL_H_
#define _UTIL_H_

int checkRange(int);
int compareString(char*, char*, char*);
int checkComma(char*);
int checkHex(char*);
int asmSeparater(char*, char*, char*, char*, char*);
void LowerToUpper(char*);
int StrToInt(char*);
int StrToHex(char*);
char* extractContent(char*);
int isDec(char*);
int isHex(char*);
int inRange(int,int,int);
int HexBitCount(int);
int insertHexAt(int,int,int);
int registerNum(char*);
void extractStr(char* dest, char* source, int start, int len);
int extractStrToHex(char* source, int start, int len);
void charArrHexCal(unsigned char* arr, int value, int len, char operation);

#endif
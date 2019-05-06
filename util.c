#include "util.h"
#include "20151619.h"

#include "math.h"

//compares command with shortcommand and longcommand and see if command matches with one of them
int compareString(char* command, char* shortcommand, char* longcommand) {
	if (!strcmp(command, shortcommand) || (longcommand != NULL && !strcmp(command, longcommand)))
		return TRUE;
	else
		return FALSE;
}

//check if adr is within the valid range
int checkRange(int adr) {
	if (adr < 0 || adr >= 0x100000)
		return FALSE;
	return TRUE;
}

//check if str has any non-hex char
int checkHex(char* str){
	for(int i=0; i<(int)strlen(str); i++){
		if((str[i] >=48 && str[i] <=57) || (str[i] >= 65 && str[i] <=70) || (str[i] >= 97 && str[i] <= 102) || str[i] == ',') 	
			continue ;
		else if(str[i] == 'X' || str[i] == 'x'){
				if(i != 0)
						if(str[i-1] == '0')
								continue;
		}
		else{
				return FALSE;
		}
	}
	return TRUE;
}

//if argCount < 0 , you need to check operand2
int asmSeparater(char* str, char* label, char* operation, char* operand, char* operand2){
	int argCount = sscanf(str,"%s %s %s %s",label,operation,operand,operand2);
	//cases with , such as LDCH BUFFER, X
	if(!checkComma(operation)){
		argCount *= -1;
		//if not all 4 inputs come in, move back
		if(argCount != -4){	
			operation[(int)strlen(operation)-1] = '\0';
			strcpy(operand2,operand);
			strcpy(operand,operation);
			strcpy(operation,label);
			memset(label,0,sizeof label);
		}
	}
	else if(!checkComma(operand)){
		argCount *= -1;
		operand[(int)strlen(operand)-1] = '\0';
	}

	//printf("[%d] ",argCount);
	switch(argCount){
		case 1:
				strcpy(operation,label);
				memset(label,0,sizeof label);
				break;
		case 2:
				strcpy(operand,operation);
				strcpy(operation,label);
				memset(label,0,sizeof label);
				break;
	}

	return argCount;
}

//changes word to uppercase
void LowerToUpper(char* word) {
	for (int i = 0; i < (int)strlen(word); i++)
		if ((int)word[i] > 96 && (int)word[i] < 123)
			word[i] -= 32;
}

//check if command finishes with a comma
int checkComma(char* command){
	int len = (int)strlen(command);
	if(command[len-1] == ',')
		return FALSE;
	return TRUE;
}

int StrToHex(char* str){
	unsigned int num;
	int cnt = sscanf(str,"%x",&num);
	if(cnt == 0)
		return -1;
		
	return num;
}

int StrToInt(char* str){
	unsigned int num;
	int cnt = sscanf(str,"%d",&num);
	if(cnt == 0)
		return -1;
		
	return num;
}

char* extractContent(char* str){
	char *temp = (char*)malloc(30);
	strncpy(temp,&(str[2]),strlen(str)-3);
	temp[strlen(str)-3] = 0;
	return temp;
}

int isDec(char* str){
	int len = (int)strlen(str);
	for(int i=0;i<len;i++)
		if(!inRange((int)'0',(int)'9',str[i]))
			return FALSE;
	return TRUE;
}

int isHex(char* str){
	int len = (int)strlen(str);
	for(int i=0;i<len;i++)
		if(str[i] == '\n')
			str[i] = '\0';
	len = (int)strlen(str);

	if(isDec(str) == TRUE)
		return TRUE;

	for(int i=0;i<len;i++)
		if(inRange((int)'a',(int)'f',str[i]) || inRange((int)'A',(int)'F',str[i]))
			return TRUE;
	return FALSE;
}

int inRange(int min,int max,int value){
	if(value >= min && value <=max)
		return TRUE;
	return FALSE;
}

int HexBitCount(int num){
	int digit = 0;
	while(num >=16){
		num /= 16;
		digit++;
	}
	return digit;
}

int HexByteCount(int num){
	return (int)(ceil((float)num/2));
}

int insertHexAt(int orig, int value, int index){
	for(int i=0;i<index;i++)
		value *= 16;
	return orig + value;
}

int registerNum(char* reg){
	if(!strcmp(reg,"A"))
		return 0;
	else if(!strcmp(reg,"X"))
		return 1;
	else if(!strcmp(reg,"L"))
		return 2;
	else if(!strcmp(reg,"PC"))
		return 8;
	else if(!strcmp(reg,"SW"))
		return 9;
	else if(!strcmp(reg,"B"))
		return 3;
	else if(!strcmp(reg,"S"))
		return 4;
	else if(!strcmp(reg,"T"))
		return 5;
	else if(!strcmp(reg,"F"))
		return 6;
	else
		return 7;
}

void extractStr(char* dest, char* source, int start, int len){
	strncpy(dest,&(source[start]),len);
	dest[len] = '\0';
	for(int i=0;i<len;i++)
		if (dest[i] == '\n' || dest[i] == ' ')
			dest[i] = '\0';
}

int extractStrToHex(char* source, int start, int len){
	char dest[300];
	strncpy(dest,&(source[start]),len);
	dest[len] = '\0';
	for(int i=0;i<len;i++)
		if (dest[i] == '\n' || dest[i] == ' ')
			dest[i] = '\0';
	return StrToHex(dest);
}

void charArrHexCal(unsigned char* arr, int value, int len, char operation){
	int converted = 0;
	int multiplier = 1;
	int temp, rem = 0;
	int repeat = ceil((float)len / 2.0);
	unsigned int result;

	for(int i = repeat-1; i>=0; i--){
		temp = arr[i];

		//last 4 bit
		converted += (temp % 16)*multiplier;
		temp /= 16;
		multiplier *= 16;

		if(len % 2 != 0 && i == 0){
			rem = temp * 16;
			continue;
		}		

		//first 4 bit
		converted += (temp % 16)*multiplier;
		temp /= 16;
		multiplier *= 16;
	}

	if(operation == '+')
		converted += value;
	else
		converted -= value;

	result = (unsigned int)converted;

	for(int i=repeat-1; i>=0; i--){
		arr[i] = result & 255;
		result /= 256;
	}
	arr[0] +=rem;
}

int bitAddressMode(int addr){
	return memory[addr] & 3;
}

int bitFormat4(int addr){
	addr += 1;
	if(memory[addr] & 0x10)
		return TRUE;
	else
		return FALSE;
}

int bitToHex(int addr,int start,int end){
	int i=0,j=1, temp=1;
	int result = 0;
	temp = 0x100;
	while(1){
		for(; i<4*j; i++){
			temp = temp >> 1;
			if(i<start)
				continue;
			else if(i>=end){
				if(memory[addr] & temp)
					result += 1;
				return result;
			}
			else
				if(memory[addr] & temp)
					result += 1;
			result *= 2;
			
		}
		if(j % 2 == 0){
			temp = 0x100;
			addr++;
		}
		j++;
	}
	return result;
}

void compareReg(int a, int b){
	if(a > b)
		registers[registerNum("SW")] = -1;
	else if(a == b)
		registers[registerNum("SW")] = 0;
	else
		registers[registerNum("SW")] = 1;
}

void printReg(){
	printf("A : %06X X : %06X\n",registers[0],registers[1]);
	printf("L : %06X PC: %06X\n",registers[2],registers[8]);
	printf("B : %06X S : %06X\n",registers[3],registers[4]);
	printf("T : %06X\n",registers[5]);
}

void writeToMem(int addr, int regNum){
	int temp = registers[regNum];
	int mask = 0xFF;
	for(int i=2;i>=0;i--){
		memory[addr+i] = temp & mask;
		temp = temp >> 8;
	}
}

int getLastByte(int regNum){
	return registers[regNum] & 0xFF;
}

void storeLastByte(int regNum, int value){
	registers[regNum] = (registers[regNum] & 0xFFFF00) + value;
}
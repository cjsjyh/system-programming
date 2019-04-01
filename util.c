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
	if(isDec(str) == TRUE)
		return TRUE;

	for(int i=0;i<len;i++)
		if(!inRange((int)'a',(int)'f',str[i]) || !inRange((int)'A',(int)'F',str[i]))
			return FALSE;
	return TRUE;
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
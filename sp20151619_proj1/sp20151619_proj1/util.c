#include "util.h"
#include "20151619.h"
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

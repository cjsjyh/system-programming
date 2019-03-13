#include "util.h"
#include "main.h"

int compareString(char* command, char* shortcommand, char* longcommand) {
	if (!strcmp(command, shortcommand) || (longcommand != NULL && !strcmp(command, longcommand)))
		return TRUE;
	else
		return FALSE;
}

int checkRange(int adr) {
	if (adr < 0 || adr >= 0x100000)
		return FALSE;
	return TRUE;
}

void LowerToUpper(char* word) {
	for (int i = 0; i < (int)strlen(word); i++)
		if ((int)word[i] > 96 && (int)word[i] < 123)
			word[i] -= 32;
}

int checkComma(char* command){
	int len = (int)strlen(command);
	if(command[len-1] == ',')
		return FALSE;
	return TRUE;
}

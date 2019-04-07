#include "20151619.h"

#include "commands.h"

#include "util.h"

//DIR Executable

int main()
{
	int isPushed = FALSE, opcode;
	int argCount=0, bfrCount=0, comCount=0;
	char command[30],fullCmd[200];
	char bfr[7][30];
	unsigned int arg1,arg2,arg3;
	int nextAdr = 0,cmaFlag=1;

	//Initialization
	lptr history = NULL;
	newsymtable = TRUE;

	optable = (hptr*)malloc(sizeof(hptr)*HASH_SIZE);
	for (int i=0; i<HASH_SIZE; i++)		
		optable[i] = NULL;

	memory=(char*)malloc(sizeof(char*)*MEM_SIZE);
	cmd_reset();	

	for(int i=0; i<7; i++)
		memset(bfr[i],0,sizeof(bfr[i]));

	hashMain("opcode.txt");

	while(TRUE)
	{
		printf("sicsim>");

		//Get Input
		fgets(fullCmd, sizeof fullCmd,stdin);
		//save with the correct format
		argCount = sscanf(fullCmd,"%s%x ,%x ,%x",command, &arg1, &arg2, &arg3);
		//save everything as string for checking
		bfrCount = sscanf(fullCmd,"%s%s%s%s%s%s%s",bfr[0],bfr[1],bfr[2],bfr[3],bfr[4],bfr[5],bfr[6]);
		//For input with input value (space) , value (space) , value
		if(!strcmp(",",bfr[2])){
			for(int i=3; i<7; i++)
				strcpy(bfr[i-1],bfr[i]);
			comCount++;
		}
		if(!strcmp(",",bfr[3])){
			for(int i=4; i<7; i++)
				strcpy(bfr[i-1],bfr[i]);
			comCount++;
		}


		if (compareString(command, "opcode", NULL) && bfrCount == 2) {
			//correct format for OPCODE [instruction] inserted
			opcode = hashSearch_opcode(bfr[1]);
			if (opcode != -1){
				printf("opcode is %X\n\n", opcode);
				linkedlist_push(&history,fullCmd);
			}
			//invalid [instruction]
			else {
				printf("Invalid Mnemonic!\n\n");
			}
			continue;
		}
		
		if (argCount != bfrCount - comCount) {
			//cases such as dump 4, hello
			cmaFlag = FALSE;
			isPushed = TRUE;
		}
		// cases such as dump 1 , 10 ,
		else if (comCount != 0 && argCount - 2 < comCount){
			cmaFlag = FALSE;
			isPushed = TRUE;
		}



		if (compareString(command, "h", "help") && argCount == 1){
			cmd_help();
			isPushed = FALSE;
		}

		else if(compareString(command,"type",NULL) && argCount == 2){
			cmd_type(bfr[1]);
			isPushed = FALSE;
		}

		else if(compareString(command,"symbol",NULL) && argCount == 1){
			if(!newsymtable)
				symtab_printAll();
			else {
				printf("Not assembled yet!\n");
				isPushed = TRUE;
			}
				
		}

		else if(compareString(command,"assemble",NULL) && argCount == 2){
			isPushed = cmd_assemble(bfr[1]);
		}

		else if (compareString(command, "d", "dir") && argCount == 1){
			cmd_dir();
			isPushed = FALSE;
		}

		else if (compareString(command, "q", "quit") && argCount == 1)
			break;

		else if (compareString(command, "hi", "history") && argCount == 1) {
			linkedlist_push(&history, fullCmd);
			linkedlist_print(history);
			isPushed = TRUE;
		}
		//Maximum number of input is 3
		else if (compareString(command, "du", "dump") && argCount <= 3 && cmaFlag) {
			switch (argCount) {
			//when only dump is inserted
			case 1:
				arg1 = INT_MIN;
				arg2 = INT_MIN;
				break;
			//when dump start is inserted
			case 2:
				arg2 = INT_MIN;
				isPushed = checkHex(bfr[1]) && checkComma(bfr[1]);
				isPushed = !isPushed;
				break;
			//when dump start, end is inserted
			case 3:
				isPushed = checkHex(bfr[1]) && checkHex(bfr[2]) && checkComma(bfr[2]);
				isPushed = !isPushed;
				break;
			}
			if (!isPushed)
				isPushed = !(cmd_dump(arg1, arg2, &nextAdr));
			//Comma at the end
			else
				printf("Invalid command!");

		}
		else if (compareString(command, "e", "edit") && argCount == 3 && cmaFlag){
			isPushed = checkComma(bfr[2]) && checkHex(bfr[1]) && checkHex(bfr[2]);
			isPushed = !isPushed;
			if (!isPushed)
				isPushed = !(cmd_edit(arg1, arg2));
			else
				printf("Invalid command!");
		}
		else if (compareString(command, "f", "fill") && argCount == 4 && cmaFlag){
			//check if the command finished with a comma or if input has non-hex
			isPushed = checkComma(bfr[3]) && checkHex(bfr[1]) && checkHex(bfr[2]) && checkHex(bfr[3]);
			isPushed = !isPushed;
			
			if (!isPushed)
				isPushed = !(cmd_fill(arg1,arg2, arg3));
			else
				printf("Invalid command!");
		}
		else if (compareString(command, "reset", NULL) && argCount == 1){
			cmd_reset();
			isPushed = FALSE;
		}

		else if (compareString(command, "opcodelist", NULL) && argCount == 1){
			hashlist_printAll(optable);
			isPushed = FALSE;
		}

		else {
			isPushed = TRUE;
			printf("Invalid Command!\n");
		}
		

		if (isPushed == TRUE)
			isPushed = FALSE;
		else
			linkedlist_push(&history,fullCmd);

		//RESET
		for(int i=0; i<7; i++)
				memset(bfr[i],0,sizeof(bfr[i]));
		argCount = bfrCount = comCount = 0;
		arg1 = arg2 = arg3 = INT_MIN;
		cmaFlag = TRUE;
		printf("\n");
	}
}


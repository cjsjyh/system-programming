#include "main.h"

//DIR Executable

int main()
{
	int isPushed = FALSE, opcode;
	int argCount, bfrCount;
	char command[30],fullCmd[200];
	char bfr1[30],bfr2[30],bfr3[30],bfr4[30],bfr5[30];
	int arg1,arg2,arg3,nextAdr = 0;
	
	//Initialization
	lptr history = NULL;

	optable = (hptr*)malloc(sizeof(hptr)*HASH_SIZE);
	for (int i=0; i<HASH_SIZE; i++)		
		optable[i] = NULL;

	memory=(char*)malloc(sizeof(char*)*MEM_SIZE);
	//for (int i=0; i<65536; i++)
	//	memory[i] = (char*)malloc(sizeof(char)*16);
	cmd_reset();	

	hashMain("opcode.txt");

	while(TRUE)
	{
		printf("sicsim>");

		//Get Input
		fgets(fullCmd, sizeof fullCmd,stdin);
		argCount = sscanf(fullCmd,"%s%x,%x,%x",command, &arg1, &arg2, &arg3);
		bfrCount = sscanf(fullCmd,"%s%s%s%s%s",bfr1, bfr2, bfr3, bfr4, bfr5);
		
		if (compareString(command, "opcode", NULL) && bfrCount == 2) {
			//correct format for OPCODE [instruction] inserted
			opcode = cmd_hashlistSearch(bfr2);
			printf("opcode: %d\n",opcode);
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

		if (argCount != bfrCount) {
			isPushed = TRUE;
			printf("Invalid Command!\n");
		}
		else if (compareString(command, "h", "help") && argCount == 1)
			cmd_help();

		else if (compareString(command, "d", "dir") && argCount == 1)
			cmd_dir();

		else if (compareString(command, "q", "quit") && argCount == 1)
			break;

		else if (compareString(command, "hi", "history") && argCount == 1) {
			linkedlist_push(&history, fullCmd);
			linkedlist_print(history);
			isPushed = TRUE;
		}
		//Maximum number of input is 3
		else if (compareString(command, "du", "dump") && argCount <= 3) {
			switch (argCount) {
			case 1:
				arg1 = INT_MIN;
				arg2 = INT_MIN;
			case 2:
				arg2 = INT_MIN;
			}
			isPushed = !(cmd_dump(arg1, arg2, &nextAdr));

		}
		else if (compareString(command, "e", "edit") && argCount == 3)
			isPushed = !(cmd_edit(arg1, arg2));

		else if (compareString(command, "f", "fill") && argCount == 4)
			isPushed = !(cmd_fill(arg1, arg2, arg3));

		else if (compareString(command, "reset", NULL) && argCount == 1)
			cmd_reset();

		
		else if (compareString(command, "opcodelist", NULL) && argCount == 1)
			hashlist_printAll(optable);

		else {
			isPushed = TRUE;
			printf("Invalid Command!\n");
		}
		

		if (isPushed == TRUE)
			isPushed = FALSE;
		else
			linkedlist_push(&history,fullCmd);

		argCount = bfrCount = 0;
		arg1 = arg2 = arg3 = INT_MIN;
		printf("\n");
	}
}



void cmd_help(){
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start,end]\n");
	printf("e[dit] address, value\nf[ill] start, end, value\nreset\n");
	printf("opcode mnemonic\nopcodelist\n");
	return;
}

void cmd_dir(){
	DIR *dir;
	struct dirent *file;
	struct stat info;
	
	if ((dir = opendir ("./")) != NULL) {
		/* print all the files and directories within directory */
		while ((file = readdir (dir)) != NULL) {
			stat(file->d_name,&info);
			
			if(info.st_mode == EXECUTABLE)
				printf ("%s*\n", file->d_name);
			else if(info.st_mode == DIRECTORY)
				printf ("%s/\n", file->d_name);
			else
				printf ("%s\n", file->d_name);

		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		printf("Error opening directory");
	}
	return;
}

int cmd_dump(int start, int end, int* nextAdr){
	int row, col, over = FALSE;
	int rowStart, rowEnd, curAdr;

	if (end == INT_MIN){
		if (start == INT_MIN)
			start = *nextAdr;
		end = start + 159;
	}
	
	if (end < start){
		printf("Invalid Range\n");
		return FALSE;
	}

	// if start or end is out of memory range
	if (!checkRange(start)){
		printf("Invalid Range\n");
		return FALSE;
	}

	rowStart = start / 16;
	rowEnd = end / 16;

	for(row = rowStart; row <= rowEnd ; row++){
		if(row * 16 > 0xFFFFF){
			over = TRUE;
			break;
		}
		printf("%05X  ", row*16);
		
		//Content
		for(col=0; col<16; col++){
			curAdr = row * 16 + col;
			if (curAdr > 0xFFFFF){
				over = TRUE;
				break;
			}
			//Range before start
			if (curAdr < start)
				printf("   ");
			//Range after end
			else if (curAdr > end)
				printf("   ");
			else
				printf("%02X ",memory[curAdr]);
		}
		printf("; ");

		//Value
		for(col=0; col<16; col++)
		{
			curAdr = row * 16 + col;
			if (curAdr > 0xFFFFF){
				over = TRUE;
				break;
			}
			//Range before start
			if (curAdr < start)
				printf(". ");
			//Range after end
			else if (curAdr > end)
				printf(". ");
			else{
				if ((int)memory[curAdr] >= 0x20 && (int)memory[curAdr] <= 0x7E){
					printf("%c ",memory[curAdr]);
				} else{
					printf(". ");
				}
			}
		}
		printf("\n");
	}
	if (over)
		*nextAdr = 0;
	else
		*nextAdr = end + 1;

	return TRUE;
}

int cmd_edit(int adr, int value){

	if (!checkRange(adr)){
		printf("Wrong Address!\n");
		return FALSE;
	}

	int row = adr / 16;
	int col = adr % 16;
	memory[row*16 + col] = value;

	return TRUE;
}

int cmd_fill(int start, int end, int value){
	int rowStart, rowEnd, row, col, curAdr;

	if(end < start){
		printf("Invalid Range!\n");
		return FALSE;
	}

	if(!(checkRange(start) || checkRange(end))){
		printf("Invalid Range!\n");
		return FALSE;
	} 
	
	rowStart = start / 16;
	rowEnd = end / 16;

	for(row = rowStart; row <= rowEnd; row++){
		for(col = 0; col < 16; col++){
			curAdr = row * 16 + col;
			if(curAdr >= start && curAdr <= end)
				memory[curAdr] = value;
		}
	}

	return TRUE;
}

void cmd_reset(){
	memset(memory,0,sizeof(char)*MEM_SIZE);
}

int cmd_hashlistSearch(char* mnem) {
	//Convert lowercase to uppercase
	LowerToUpper(mnem);
	int index = hashfunction(mnem);
	printf("index: %d\n",index);
	hptr temp = optable[index];
	while (temp != NULL) {
		if (!strcmp(mnem, temp->mnem))
			return temp->opcode;
		temp = temp->next;
	}
	return -1;
}

int compareString(char* command, char* shortcommand, char* longcommand){
	if (!strcmp(command,shortcommand) || (longcommand != NULL && !strcmp(command,longcommand)))
		return TRUE;
	else
		return FALSE;
}

int checkRange(int adr) {
	if (adr < 0 || adr >= 0x100000)
		return FALSE;
	return TRUE;
}

void LowerToUpper(char* word){
	for(int i=0; i<(int)strlen(word); i++)
		if ((int)word[i]>96 && (int)word[i]<123)
			word[i] -= 32;
}

void linkedlist_push(lptr* head,char* command){
	lptr temp = *head;
	lptr newNode = (lptr)malloc(sizeof(linkedlist));
	
	newNode->next = NULL;
	strcpy(newNode->command,command);

	if(temp != NULL){
		while(temp->next != NULL)
			temp = temp->next;	
		temp->next = newNode;
	}
	else{
		*head = newNode;
	}

	return;
}

void linkedlist_print(lptr head){
	int count = 1;
	lptr temp = head;

	while(temp != NULL){
		printf("%-4d   %s",count++,temp->command);
		temp = temp->next;
	}

	return;
}

void hashMain(char* fname){
	FILE *fp = fopen(fname, "r");
	int opcode,index;
	char mnem[30], format[30];

	if (fp != NULL){
		while(fscanf(fp,"%X %s %s",&opcode,mnem,format) != EOF){
			index = hashfunction(mnem);
			hashlist_push(&(optable[index]),mnem,opcode);
		}
	}
	else{
		printf("Cannot open File\n!");
	}
}

int hashfunction(char* mnem){
	int i,sum=0;
	for(i=0; i<(int)strlen(mnem); i++)
		sum += (int)mnem[i];
	return sum % HASH_SIZE;
}

void hashlist_push(hptr *head,char* mnem,int opcode){
	hptr temp = *head;

	hptr newNode = (hptr)malloc(sizeof(hashlist));
	newNode->next = NULL;
	strcpy(newNode->mnem,mnem);
	newNode->opcode = opcode;


	if(*head != NULL){
		while(temp->next != NULL)
			temp = temp->next;	
		temp->next = newNode;
	}else{
		*head = newNode;
	}

	return;
}

void hashlist_printAll(hptr *head){
	int i, isFirst;
	hptr temp;

	for(i=0; i<HASH_SIZE; i++){
		temp = head[i];
		isFirst = TRUE;
		if(temp != NULL){
			printf("%d : ",i);
			while(temp != NULL){
				if(!isFirst)
					printf(" -> ");
				printf("[%s, %X]",temp->mnem,temp->opcode);
				temp = temp->next;
				isFirst = FALSE;
			}
			printf("\n");
		}

	}
}

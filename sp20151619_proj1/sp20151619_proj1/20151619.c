#include "20151619.h"

//DIR Executable

int main()
{
	int isPushed = FALSE, opcode;
	int argCount=0, bfrCount=0, comCount=0;
	char command[30],fullCmd[200];
	char bfr[7][30];
	unsigned int arg1,arg2,arg3;
	int nextAdr = 0;
	
	//Initialization
	lptr history = NULL;

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
			opcode = cmd_hashlistSearch(bfr[1]);
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
		//cases such as dump 4, hello
		if (argCount != bfrCount - comCount) {
			isPushed = TRUE;
			printf("Invalid Command!\n");
		}
		// cases such as dump 1 , 10 ,
		else if (comCount != 0 && argCount - 2 < comCount){
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
		else if (compareString(command, "e", "edit") && argCount == 3){
			isPushed = checkComma(bfr[2]) && checkHex(bfr[1]) && checkHex(bfr[2]);
			isPushed = !isPushed;
			if (!isPushed)
				isPushed = !(cmd_edit(arg1, arg2));
			else
				printf("Invalid command!");
		}
		else if (compareString(command, "f", "fill") && argCount == 4){
			//check if the command finished with a comma or if input has non-hex
			isPushed = checkComma(bfr[3]) && checkHex(bfr[1]) && checkHex(bfr[2]) && checkHex(bfr[3]);
			isPushed = !isPushed;
			
			if (!isPushed)
				isPushed = !(cmd_fill(arg1,arg2, arg3));
			else
				printf("Invalid command!");
		}
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

		//RESET
		for(int i=0; i<7; i++)
				memset(bfr[i],0,sizeof(bfr[i]));
		argCount = bfrCount = comCount = 0;
		arg1 = arg2 = arg3 = INT_MIN;
		printf("\n");
	}
}

//appends new node with command to the end of head linked list
void linkedlist_push(lptr* head,char* command){
	lptr temp = *head;
	lptr newNode = (lptr)malloc(sizeof(linkedlist));
	
	newNode->next = NULL;
	strcpy(newNode->command,command);

	if(temp != NULL){
		//goes to the last node
		while(temp->next != NULL)
			temp = temp->next;	
		temp->next = newNode;
	}
	else{
		*head = newNode;
	}

	return;
}

//prints all the nodes in the linked list my moving one node at a time
void linkedlist_print(lptr head){
	int count = 1;
	lptr temp = head;

	while(temp != NULL){
		printf("%-4d   %s",count++,temp->command);
		temp = temp->next;
	}

	return;
}

//main function to create initial hashtable
void hashMain(char* fname){
	FILE *fp = fopen(fname, "r");
	int opcode,index;
	char mnem[30], format[30];

	if (fp != NULL){
		while(fscanf(fp,"%X %s %s",&opcode,mnem,format) != EOF){
			//finds the index for mnemonic to go in the hashtable
			index = hashfunction(mnem);
			//appends that mnemonic to the table
			hashlist_push(&(optable[index]),mnem,opcode,format);
		}
	}
	else{
		printf("Cannot open File\n!");
	}
}

//returns index number for the mnemonic to go into in the hashtable
int hashfunction(char* mnem){
	int i,sum=0;
	for(i=0; i<(int)strlen(mnem); i++)
		sum += (int)mnem[i];
	return sum % HASH_SIZE;
}

//creates a new node with mnemonic, opcode and format. Then inserts the new node the end of head list
void hashlist_push(hptr *head,char* mnem,int opcode, char* format){
	hptr temp = *head;

	//make new node
	hptr newNode = (hptr)malloc(sizeof(hashlist));
	newNode->next = NULL;
	strcpy(newNode->mnem,mnem);
	strcpy(newNode->format,format);
	newNode->opcode = opcode;


	if(*head != NULL){
		//go to the end of head
		while(temp->next != NULL)
			temp = temp->next;	
		//insert to new node
		temp->next = newNode;
	}else{
		*head = newNode;
	}

	return;
}

//prints whole hashtable
void hashlist_printAll(hptr *head){
	int i, isFirst;
	hptr temp;

	for(i=0; i<HASH_SIZE; i++){
		temp = head[i];
		isFirst = TRUE;
		//if an index is not empty
		if(temp != NULL){
			printf("%d : ",i);
			//iterate through the list
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

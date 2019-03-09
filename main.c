#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define EXECUTABLE 33279
#define DIRECTORY 16895

typedef struct linkedlist* lptr;
typedef struct linkedlist {
	char command[20];
	lptr next;
}linkedlist;
void linkedlist_push(lptr, char*);
void linkedlist_print(lptr);

char **memory;
void cmd_dir();
void cmd_help();
void cmd_dump(int,int,int*);
void cmd_edit(int,int);
void cmd_fill(int,int,int);
int compareString(char*,char*,char*);




int main()
{
	int isPushed = FALSE;
	int argCount, bfrCount;
	char command[30],fullCmd[200];
	char bfr1[30],bfr2[30],bfr3[30],bfr4[30],bfr5[30];
	int arg1,arg2,arg3,nextAdr = 0;
	
	lptr history = (lptr)malloc(sizeof(linkedlist));
	
	memory=(char**)malloc(sizeof(char*)*65536);
	for (int i=0; i<65536; i++)
	{
		memory[i] = (char*)malloc(sizeof(char)*16);
		memset(memory[i],0x6A,sizeof(char)*16);
		
	}
	
	while(TRUE)
	{
		printf("sicsim>");

		//Get Input
		fgets(fullCmd, sizeof fullCmd,stdin);
		argCount = sscanf(fullCmd,"%s %x, %x, %x",command,&arg1,&arg2,&arg3);
		bfrCount = sscanf(fullCmd,"%s %s %s %s %s",bfr1,bfr2,bfr3,bfr4,bfr5);
		printf("%s %x %x\n",command,arg1,arg2);
		
		if(argCount != bfrCount){
			isPushed = TRUE;
			printf("Invalid Command!\n");
		}
		else if (compareString(command,"h","help") && argCount == 1)
			cmd_help();
		else if(compareString(command,"d","dir") && argCount == 1)
			cmd_dir();

		else if(compareString(command,"q","quit") && argCount == 1)
			break;

		else if(compareString(command,"hi","history") && argCount == 1){
			linkedlist_push(history,command);
			linkedlist_print(history);
			isPushed = TRUE;
		}
		//Maximum number of input is 3
		else if(compareString(command,"du","dump") && argCount <= 3){
			switch(argCount){
				case 1:
					arg1 = INT_MIN;
					arg2 = INT_MIN;
				case 2:
					arg2 = INT_MIN;
			}
			cmd_dump(arg1,arg2,&nextAdr);
		
		}			
		else if(compareString(command,"e","edit") && argCount == 3)
			cmd_edit(arg1,arg2);

		else if(compareString(command,"f","fill"))
			cmd_fill(arg1,arg2,arg3);
		
/*
		else if(compareString(command,"reset",NULL))

		else if(compareString(command,"opcode mnemonic",NULL))

		else if(compareString(command,"opcodelist",NULL))
*/
		else{
			isPushed = TRUE;
			printf("Invalid Command!\n");
		}


		if (isPushed == TRUE)
			isPushed = FALSE;
		else
			linkedlist_push(history,command);

		argCount = bfrCount = 0;
		arg1 = arg2 = arg3 = INT_MIN;
		printf("\n");
	}
}



void cmd_help(){
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start,end]\n");
	printf("e[dit] address, value\n,f[ill] start, end, value\nreset\n");
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

void cmd_dump(int start, int end, int* nextAdr){
	int row, col;
	int rowStart, rowEnd, curAdr;

	if (end == INT_MIN){
		if (start == INT_MIN)
			start = *nextAdr;
		end = start + 159;
	}
	
	printf("%d %d\n",start,end);
	if (end < start){
		printf("Invalid Range\n");
		return;
	}
	
	rowStart = start / 16;
	rowEnd = end / 16;

	for(row = rowStart; row <= rowEnd ; row++){
		curAdr = row * 16 + col; 

		//Row Address
		printf("%05X  ", row*16);
		
		//Content
		for(int col=0; col<16; col++){

			//printf("[%d %d]",row,col);

			//Range outside of memory
			if (curAdr < 0 || curAdr >= 0x100000)
				printf("   ");
			//Range before start
			else if (curAdr < start)
				printf("   ");
			//Range after end
			else if (curAdr > end)
				printf("   ");
			else
				printf("%02X ",memory[row][col]);
		}
		printf("; ");

		//Value
		for(int col=0; col<16; col++)
		{
			//Range outside of memory
			if (curAdr < 0 || curAdr >= 0x100000)
				printf(". ");
			//Range before start
			else if (curAdr < start)
				printf(". ");
			//Range after end
			else if (curAdr > end)
				printf(". ");
			else{
				if ((int)memory[row][col] >= 0x20 && (int)memory[row][col] <= 0x7E){
					printf("%c ",memory[row][col]);
				} else{
					printf(". ");
				}
			}
		}
		printf("\n");
	}

	*nextAdr = end + 1;
}

void cmd_edit(int addr, int value){

	if (addr < 0 || addr >= 0x100000){
		printf("Wrong Address!\n");
		return;
	}

	int row = addr / 16;
	int col = addr % 16;
	memory[row][col] = value;

	return;
}

void cmd_fill(int start, int end, int value){
	int rowStart, rowEnd, row, col, curAdr;
	rowStart = start / 16;
	rowEnd = end / 16;

	for(row = rowStart; row <= rowEnd; row++){
		for(col = 0; col < 16; col++){
			curAdr = row * 16 + col;
			if(curAdr >= start && curAdr <= end)
				memory[row][col] = value;
		}
	}
}

int compareString(char* command, char* shortcommand, char* longcommand){
	if (!strcmp(command,shortcommand) || !strcmp(command,longcommand))
		return TRUE;
	else
		return FALSE;
}

void linkedlist_push(lptr head,char* command){
	lptr temp = head;
	lptr newNode = (lptr)malloc(sizeof(linkedlist));

	while(temp->next != NULL)
		temp = temp->next;	
	
	newNode->next = NULL;
	strcpy(newNode->command,command);

	temp->next = newNode;

	return;
}

void linkedlist_print(lptr head){
	int count = 1;
	lptr temp = head->next;
	while(temp != NULL){
		printf("%4d   %s\n",count++,temp->command);
		temp = temp->next;
	}
}

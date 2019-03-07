#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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
void cmd_dump(int, int);
int cmd_dir();
void cmd_help();
int compareString(char*, char*, char*);

int main()
{
	int isPushed = FALSE,argCount;
	int args[5];
	char *command,fullCmd[50];
	
	lptr history = (lptr)malloc(sizeof(linkedlist));
	
	memory=(char**)malloc(sizeof(char*)*65536);
	for (int i=0; i<65536; i++)
	{
		memory[i] = (char*)malloc(sizeof(char)*16);
		memset(memory[i],0x61,sizeof(char)*16);
		
	}
	
	while(TRUE)
	{
		printf("sicsim>");

		//Get Input
		fgets(fullCmd, sizeof fullCmd,stdin);
		fullCmd[(int)strlen(fullCmd)-1] = ' ';
		command = strtok(fullCmd," ");

		//DUMP!
		if (compareString(command,"h","help"))
			cmd_help();
		else if(compareString(command,"d","dir"))
			cmd_dir();

		else if(compareString(command,"q","quit"))
			break;

		else if(compareString(command,"hi","history")){
			linkedlist_push(history,command);
			linkedlist_print(history);
			isPushed = TRUE;
		}
		else if(compareString(command,"du","dump")){
			linkedlist_push(history,command);
			isPushed = TRUE;
			
			while(command != NULL){
				if(argCount++ != 0)
					args[argCount] = (int)(*command);
				printf("%s\n",command);
				command = strtok(NULL," ");
			}
			cmd_dump(0,2);
		
		}			
/*
		else if(compareString(command,"e","edit"))


		else if(compareString(command,"f","fill"))

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

		argCount = 0;
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
	
	printf("%d %d\n",S_IXUSR,S_IFDIR);

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

void cmd_dump(int start, int end){
	for(int i=0; i<10; i++){
		printf("%05x  ",start + 16*i);
		for(int j=0; j<16; j++)
			printf("%02x ",memory[i][j]);
		
		for(int j=0; j<16; j++)
		{
			if ((int)memory[i][j] >= 0x20 && (int)memory[i][j] <= 0x7E){
				printf("%c ",memory[i][j]);
			} else{
				printf(". ");
			}
		}
		printf("\n");
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

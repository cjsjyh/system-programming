#include "ds.h"
#include "20151619.h"

#include <string.h>
#include <stdlib.h>

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
int hashfunction(char* str){
	int i,sum=0;
	for(i=0; i<(int)strlen(str); i++)
		sum += (int)str[i];
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

void symtab_push(symtab **head,char* label,int addr){
	symtab *temp = *head;

	//make new node
	symtab* newNode = (symtab*)malloc(sizeof(symtab));
	strcpy(newNode->label,label);
	newNode->addr = addr;
	newNode->next = NULL;

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

void symtab_print(symtab *head){
    while(head != NULL){
        printf("\t%s %X\n",head->label,head->addr);
        head = head->next;
    }
}

int symfunction(char* str){
	return (int)str[0] - (int)'A';
}

void interm_push(intermptr *head,intermptr newNode){
	intermptr temp = *head;

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
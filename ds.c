#include "ds.h"
#include "20151619.h"

#include <string.h>
#include <stdlib.h>

void extsymtab_push(extsymtab** head, char* symbol, int addr, int length){
	extsymtab* temp;
	extsymtab* newNode = (extsymtab*)malloc(sizeof(extsymtab));
	strcpy(newNode->label,symbol);
	newNode->addr = addr;
	newNode->length = length;
	newNode->next = NULL;

	if(*head == NULL)
		*head = newNode;
	else{
		temp = *head;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = newNode;
	}
}

extsymptr extsymtab_search(extsymtab* head, char* symbol){
	if (head == NULL)
		return NULL;
	else{
		while(head != NULL){
			if (!strcmp(head->label,symbol))
				return head;
			head = head->next;
		}
	}
	return NULL;
}

void extsymtab_printAll(extsymtab* head){
	printf("control 	symbol		address 	length\nsection 	name\n");
	printf("------------------------------------------------------\n");
	while(head != NULL){
		if(head->length < 0){
			printf("\t\t%-7s\t\t",head->label);
			printf("%04X\n",head->addr);
		}
		else{
			printf("%-7s\t\t\t\t",head->label);
			printf("%04X\t\t",head->addr);
			printf("%04X\n",head->length);
		}
		head = head -> next;
	}
}

int bplist_push(int addr){
	bpptr temp;
	bpptr newNode = (bpptr)malloc(sizeof(bplist));
	newNode->next = NULL;
	newNode->addr = addr;

	if (breakpoints == NULL){
		breakpoints = newNode;
		return 1;
	}
	else if(breakpoints->addr > addr){
		newNode->next = breakpoints;
		breakpoints = newNode;
		return 1;
	}
	else if(breakpoints->next == NULL){
		//same address already exists
		if(breakpoints->addr == addr)
			return 0;
			
		breakpoints->next = newNode;
		return 1;
	}
	else{
		temp = breakpoints;
		while(temp->next->addr <= addr){
			//same address already exists
			if(temp->next->addr == addr)
				return 0;
			
			if(temp->next->next == NULL){
				temp->next->next = newNode;
				return 1;
			}
			temp = temp -> next;
		}
		newNode->next = temp->next;
		temp->next = newNode;
	}
}

int bplist_search(int addr){
	bpptr temp = breakpoints;
	while( temp != NULL ){
		if(temp->addr == addr)
			return TRUE;
		temp = temp->next;
	}
	return FALSE;
}

void bplist_printAll(){
	bpptr temp = breakpoints;
	if(breakpoints == NULL){
		printf("No breakpoints set.\n");
		return;
	}
	printf("breakpoints\n-----------\n");
	while(temp != NULL){
		printf("%X\n",temp->addr);
		temp = temp->next;
	}
}

void bplist_clear(){
	if (breakpoints == NULL)
		return;

	bpptr temp = breakpoints;
	bpptr temp2 = temp->next;
	while(temp2 != NULL){
		free(temp);
		temp = temp2;
		temp2 = temp2->next;
	}
	breakpoints = NULL;
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

int symtab_push(symtab **head,char* label,int addr){
	symtab *temp;
	symtab *prev;

	//make new node
	symtab* newNode = (symtab*)malloc(sizeof(symtab));
	strcpy(newNode->label,label);
	newNode->addr = addr;
	newNode->next = NULL;


	if(*head != NULL){
		//head에 삽입하는 경우
		if (strcmp((*head)->label, label) < 0){
			newNode->next = *head;
			*head = newNode;
			return FALSE;
		}
		else{
			temp = (*head)->next;
			prev = *head;
		}
		//Search for the right place to go
		while(temp != NULL){
			//duplicate symbol
			if(!strcmp(temp->label,label))
				return TRUE;
			if(strcmp(temp->label,label) < 0){
				prev->next = newNode;
				newNode ->next = temp;
				return FALSE;
			}
			prev = temp;
			temp = temp->next;
		}
		//insert to new node
		prev->next = newNode;
	}else{
		*head = newNode;
	}

	return FALSE;
}

int symtab_search(symtab* head,char* label){
	while(head != NULL){
		if(!strcmp(head->label,label))
			return head->addr;
		head = head->next;
	}
	return -1;
}

void symtab_printAll(){
	for(int i=SYM_SIZE-1;i>=0;i--)
		symtab_print(symboltable[i]);
}

void symtab_print(symtab *head){
    while(head != NULL){
        printf("\t%s %X\n",head->label,head->addr);
        head = head->next;
    }
}

int symfunction(char* str){
	int index = (int)str[0] - (int)'A';
	if(index < 0)
		return 0;
	else
		return index;
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
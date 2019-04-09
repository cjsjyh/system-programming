#ifndef _DS_C_
#define _DS_C_

#define HASH_SIZE 20
#define SYM_SIZE 26

typedef struct interm* intermptr;
typedef struct interm {
    int addr;
	int format;
	int argCount;
    char operation[30];
    char operand[20];
    char operand2[20];
	char label[30];
	char line[200];
    intermptr next;
}interm;

typedef struct linkedlist* lptr;
typedef struct linkedlist {
	char command[100];
	lptr next;
}linkedlist;
void linkedlist_push(lptr*, char*);
void linkedlist_print(lptr);

typedef struct symtable* sptr;
typedef struct symtable {
	char label[30];
	unsigned int addr;
	sptr next;
}symtable;

typedef struct hashlist* hptr;
typedef struct hashlist {
	char mnem[20];
	int opcode;
	char format[20];
	hptr next;
}hashlist;
void hashMain(char*);
int hashfunction(char*);
void hashlist_push(hptr*, char*, int, char*);
void hashlist_printAll(hptr*);

typedef struct symtab* symptr;
typedef struct symtab {
	char label[30];
	int addr;
	symptr next;
}symtab;
int symfunction(char*);
int symtab_push(symtab**,char*,int);
void symtab_print(symptr);
void symtab_printAll();
int symtab_search(symtab*,char*);

void interm_push(intermptr*, intermptr);

#endif
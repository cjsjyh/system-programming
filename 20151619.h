#ifndef _20151619_H_
#define _20151619_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#include "commands.h"

#define TRUE 1
#define FALSE 0
#define HASH_SIZE 20
#define MEM_SIZE 65536 * 16
#define WORD 3
#define BYTE 1

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

void interm_push(intermptr);

unsigned char *memory;
hashlist **optable;
intermptr intermediate;

#endif


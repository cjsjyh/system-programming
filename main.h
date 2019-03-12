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
#define HASH_SIZE 20
#define MEM_SIZE 65536 * 16

typedef struct linkedlist* lptr;
typedef struct linkedlist {
	char command[100];
	lptr next;
}linkedlist;
void linkedlist_push(lptr*, char*);
void linkedlist_print(lptr);

typedef struct hashlist* hptr;
typedef struct hashlist {
	char mnem[20];
	int opcode;
	hptr next;
}hashlist;
void hashMain(char*);
int hashfunction(char*);
void hashlist_push();
void hashlist_printAll();

char *memory;
hashlist **optable;

void cmd_dir();
void cmd_help();
void cmd_reset();
int cmd_dump(int, int, int*);
int cmd_edit(int, int);
int cmd_fill(int, int, int);
int cmd_hashlistSearch(char*);

int checkRange(int);
int compareString(char*, char*, char*);
void LowerToUpper(char*);
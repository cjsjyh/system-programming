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
#define MEM_SIZE 65536 * 16
#define WORD 3
#define BYTE 1
#define MAX_TEXT_LINE 69

#include "ds.h"

unsigned char *memory;
hashlist **optable;
symptr *symboltable;
bpptr breakpoints;
int newsymtable;
int progaddr;
// | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8  | 9  |
// | A | X | L | B | S | T | F |   | PC | SW |
int registers[10];

int endaddr[20][2];
int endindex;

#endif


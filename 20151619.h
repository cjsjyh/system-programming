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
int newsymtable;

#endif


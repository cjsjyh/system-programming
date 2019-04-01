#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define CHAR 0
#define HEX 1
#define DEC 2

void cmd_dir();
void cmd_help();
void cmd_type(char*);
void cmd_assemble(char*);
void cmd_reset();
int cmd_dump(int, int, int*);
int cmd_edit(int, int);
int cmd_fill(int, int, int);
int hashSearch_opcode(char*);
char* hashSearch_format(char*);

typedef struct interm* intermptr;
typedef struct interm {
    int addr;
	int format;
	int argCount;
    char operation[30];
    char operand[20];
    char operand2[20];
    intermptr next;
}interm;

#endif
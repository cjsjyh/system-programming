#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define CHAR 0
#define HEX 1
#define DEC 2

void cmd_dir();
void cmd_help();
int cmd_loader(char**,int);
void cmd_type(char*);
int cmd_assemble(char*);
void cmd_reset();
int cmd_dump(int, int, int*);
int cmd_edit(int, int);
int cmd_fill(int, int, int);
int hashSearch_opcode(char*);
char* hashSearch_format(char*);
int addressingMode(char*);


#endif
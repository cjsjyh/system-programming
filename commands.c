#include "commands.h"
#include "util.h"
#include "20151619.h"
#include <math.h>
#include "ds.h"

//Prints Help
void cmd_help() {
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start,end]\n");
	printf("e[dit] address, value\nf[ill] start, end, value\nreset\n");
	printf("opcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
	return;
}

void cmd_type(char* filename){
	FILE *fp = fopen(filename,"r");
	char line[100]={0,};
	if (fp == NULL){
		printf("File doesn't exist!\n");
		return;
	}
	while(fgets(line,100,fp) != NULL)
			printf("%s",line);
	fclose(fp);
	return;
}

void cmd_assemble(char* filename){
	FILE *fp = fopen(filename, "r");
	char line[200]={0,};
	char label[30], operand[30], operand2[30], operation[30], *tempFormat;
	char* tempContent;
	int argCount, num, numCount, base, hashindex;
	int isFirst = TRUE, errorFlag = FALSE;
	int opcode,format=-1, locctr=0,i=0;
	
	intermptr intermediate=NULL;
	intermptr newinterm;

	//PASS 1
	if (fp == NULL){
		printf("File doesn't exist!\n");
		return;
	}

	//if symbol table is alreay set, clear
	if(!newsymtable){
		for(int i = SYM_SIZE; i>=0; i--)
			free(symboltable[i]);
		free(symboltable);
		symboltable = NULL;
	}

	symboltable = (symptr*)calloc(SYM_SIZE,sizeof(symptr)*SYM_SIZE);

	//Get 1 line at a time from file until NULL
	while(fgets(line,200,fp) != NULL && errorFlag == FALSE){
		//empty line
		if(!strcmp(line,"") || !strcmp(line,"\n"))
			continue;
		//ignore comment line
		i=0;
		while(line[i] == ' ' || line[i] =='\t')
			i++;
		if(line[i] == '.')
			continue;
		
		argCount = asmSeparater(line,label,operation,operand,operand2);

		if(isFirst){
			if(!strcmp(operation,"START"))
				locctr = StrToHex(operand); 
			else
				locctr = 0;
			isFirst = FALSE;
		}

		if(strcmp(label,"")){
			int index = symfunction(label);
			printf("[label:%s] %s",label,line);
			if(symtab_push(&(symboltable[index]), label, locctr) == -1){
				errorFlag = TRUE;
				printf("%s already exists!\n",label);
			}
		}
			

		//make new node for a line of intermediate file
		newinterm = (intermptr)malloc(sizeof(interm));
		newinterm->addr = locctr;
		newinterm->next = NULL;
		newinterm->argCount = argCount;

		printf("[%X] %s",locctr,line);

		//Handle format 4 exception
		if(operation[0] == '+'){
			format = 4;
			tempFormat = hashSearch_format(&(operation[1]));
			opcode = hashSearch_opcode(&(operation[1]));
			strcpy(newinterm->operation,&(operation[1]));
		}
		else{
			tempFormat = hashSearch_format(operation);
			opcode = hashSearch_opcode(operation);
			strcpy(newinterm->operation,operation);
		}

		//Set format for 1,2,3
		if(tempFormat != NULL && format != 4){
			if(!strcmp(tempFormat,"3/4"))
				format = 3;
			else
				format = (int)tempFormat[0] - (int)'0';
		}

		//make new node for a line of intermediate file
		newinterm->format = format;
		strcpy(newinterm->operand,operand);
		strcpy(newinterm->operand2,operand2);
		interm_push(&intermediate,newinterm);

		

		//increase LOCCTR
		if(format != -1){
			//printf("[%X]%s is format %d\n",locctr,operation,format);
			locctr += format;
		}//if
		//directives without opcode such as RESB, RESW
		else{
			if(!strcmp(operation,"RESW")){
				num = StrToInt(operand);
				//printf("[%X]RESW operand %d\n",locctr,num);
				locctr += num * 3;
			}
			else if(!strcmp(operation,"RESB")){
				num = StrToInt(operand);
				//printf("\n[%X]RESB operand %d\n",locctr,num);
				locctr += num;
			}
			else if(!strcmp(operation, "BYTE")){
				//Character input
				tempContent = extractContent(operand);
				if(operand[0] == 'X'){
					num = (int)strlen(tempContent);
					locctr += ceil((float)num/2);
					//printf("[%X]BYTE operand %X\n",locctr,num);
					//get size of hex
				}
				else if(operand[0] == 'C'){
					//printf("[%X]BYTE operand %s\n",locctr,tempContent);
					locctr += strlen(tempContent);
				}
				else
					printf("What is this input? %s\n",operand);
			}
			else if(!strcmp(operation, "WORD")){
				num = StrToInt(operand);	
				//printf("[%X]WORD operand %d\n",locctr,num);
				locctr += 3;
			}
			else if(!strcmp(operation,"END")){
				//printf("[%X]END operand %d\n",locctr,num);
			}
			else if(!strcmp(operation,"BASE")){
				//hashindex = symfunction(operand);
				//base = symtab_search(symboltable[hashindex],operand);
			}
			//unknown directive
			else{
				//printf("!!ELSE!! %s\n",operation);
				continue;
			}

		}//else

		if(errorFlag){
			printf("Error while assembling!\n");
			return;
		}

		//reset
		format = -1;
		memset(operation, 0, sizeof operation);
		memset(label, 0, sizeof label);
		memset(operand, 0, sizeof operand);
		memset(operand2, 0, sizeof operand2);

	}//while
	fclose(fp);

	printf("\n\n\n");

	symtab_printAll();

	//PASS 2
	if (intermediate == NULL){
		printf("No lines to assemble\n");
	}
	else{
		int offset, pc, hashindex;
		unsigned int obj12=0,obj3=0,obj4=0;
		char strcontent[30];
		intermptr curline;
		curline = intermediate;

		while(curline != NULL && errorFlag == FALSE){
			//lines that don't produce opcodes
			if(compareString(curline->operation,"RESW","RESB")){
				curline = curline->next;
				continue;
			}
			else if(compareString(curline->operation,"START","END")){
				curline = curline->next;
				continue;
			}
			else if(compareString(curline->operation,"BASE",NULL)){
				curline = curline->next;
				continue;
			}

			//initialize variables
			obj12 = obj3 = obj4 = 0;

			//first 2 digits of object code
			obj12 = hashSearch_opcode(curline->operation);

			if(curline->format == 2){
				if(!strcmp(curline->operand,"SVC")){
					int temp = StrToHex(curline->operand);
					obj3 = insertHexAt(obj3,temp,1);
				}
				else{
					int temp = registerNum(curline->operand);
					//if operand isn't a register
					if(temp == -1){
						
						errorFlag = TRUE;
						continue;
					}
					obj3 = insertHexAt(obj3,temp,1);
					//If second operand exists
					if(curline->argCount < 0){
						if(compareString(curline->operand2,"SHIFTL","SHIFTR")){
							temp = StrToHex(curline->operand2);
							obj3 = insertHexAt(obj3,temp,0);
						}
						else{
							temp = registerNum(curline->operand2);
							//if operand isn't a register
							if(temp == -1){
								
								errorFlag = TRUE;
								continue;
							}
							obj3 = insertHexAt(obj3,temp,0);
						}
						
					}
				}//else
			}
			else if(curline->format >= 3){
				//set e flag for format 4
				if(curline->format == 4)
						obj3++;
				if(abs(curline->argCount) > 1){
					//set flag for addressing mode
					obj12 += addressingMode(curline->operand);
					//Set flag for indexed addressing
					if(!strcmp(curline->operand2,"X"))
						obj3 += 8;
					
					//remove first character
					if(curline->operand[0] == '#' || curline->operand[0] == '@')
						strcpy(strcontent,curline->operand +1);
					else
						strcpy(strcontent,curline->operand);

					//Check if operation is LDB and set base variable
					hashindex = symfunction(strcontent);
					//printf("index = %d operand %s\n",hashindex,strcontent);
					offset = symtab_search(symboltable[hashindex],strcontent);
					if(!strcmp(curline->operation,"LDB"))
						base = offset;
					//operand doesn't exist in the symbol table
					if(offset == -1){
						int temp = StrToInt(strcontent);
						if (temp != -1)
							obj4 += temp;
						else{
							printf("%s error here\n",strcontent);
							errorFlag = TRUE;
							continue;
						}
					}
					else{
						//if format4, direct addressing
						if(curline->format == 4)
							obj4 = offset;
						else{
							//check for PC relative
							pc = curline->next->addr;
							//printf("OFFSET: %X PC: %X result: %X ",offset,pc,offset-pc);
							if(offset - pc >= -2048 && offset - pc <= 2047){
								//use pc relative
								obj4 = offset - pc;
								obj3 += 2;
								//printf("PC RELATIVE\n");
							}
							else{
								//use base relative
								obj4 = offset - base;
								obj3 += 4;
								//printf("BASE RELATIVE\n");
							}
						}
					}
				}//if argCount > 1
				else{
					obj12 += 3;
				}	
			}
			else if(!strcmp(curline->operation, "BYTE")){
				//Character input
				char *tempContent = extractContent(curline->operand);
				printf("[%04X] ",curline->addr);
				if(curline->operand[0] == 'X'){
					printf("%s\n",tempContent);
				}
				else if(curline->operand[0] == 'C'){
					for(int i=0; i<strlen(tempContent); i++)
						printf("%2X",(int)(tempContent[i]));
					printf("\n");
				}
				else
					printf("What is this input? %s\n",curline->operand);
				
				curline = curline->next;
				continue;
			}//format3,4 if
			printf("[%04X] ",curline->addr);
			
			switch(curline->format){
				case 1:
					printf("%02X\n",obj12);
					break;
				case 2:
					printf("%02X%02X\n",obj12,obj3);
					break;
				case 3:
					printf("%02X%01X%03X\n",obj12,obj3,obj4%(1<<12));
					break;
				case 4:
					printf("%02X%01X%05X\n",obj12,obj3,obj4%(1<<20));
					break;
				default:
					printf("OTHER!: %s\n",curline->operation);
			}
			
			curline = curline->next;
		}//while
	}
	
	if(errorFlag)
		printf("ERROR!!\n");
	newsymtable = FALSE;
}

//Prints files in current directory
void cmd_dir() {
	DIR *dir;
	struct dirent *file;
	struct stat info;

	if ((dir = opendir("./")) != NULL) {
		/* print all the files and directories within directory */
		while ((file = readdir(dir)) != NULL) {
			stat(file->d_name, &info);
			//If file is a directory
			if(S_ISDIR(info.st_mode))
					printf("%s/\n",file->d_name);
			//If file is executable
			else if(info.st_mode & S_IXUSR)
					printf("%s*\n", file->d_name);
			//other files
			else
					printf("%s\n",file->d_name);
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		printf("Error opening directory");
	}
	return;
}

//Prints memory
int cmd_dump(int start, int end, int* nextAdr) {
	int row, col, over = FALSE;
	int rowStart, rowEnd, curAdr;

	//if ending addr is not inserted
	if (end == INT_MIN) {
		//if starting addr is not inserted
		if (start == INT_MIN)
			start = *nextAdr;
		end = start + 159;
	}
	else{
		//if inserted range end is out of bound
		if (checkRange(end) == FALSE || checkRange(start) == FALSE){
			printf("Invalid Range\n");
			return FALSE;
		}
		
	}
	
	//if start addr is bigger than end addr
	if (end < start) {
		printf("Invalid Range\n");
		return FALSE;
	}

	// if start is out of memory range
	if (!checkRange(start)) {
		printf("Invalid Range\n");
		return FALSE;
	}

	rowStart = start / 16;
	rowEnd = end / 16;

	for (row = rowStart; row <= rowEnd; row++) {
		// if row is out of range
		if (row * 16 > 0xFFFFF) {
			over = TRUE;
			break;
		}
		printf("%05X  ", row * 16);

		//Content
		for (col = 0; col < 16; col++) {
			curAdr = row * 16 + col;
			if (curAdr > 0xFFFFF) {
				over = TRUE;
				break;
			}
			//Range before start
			if (curAdr < start)
				printf("   ");
			//Range after end
			else if (curAdr > end)
				printf("   ");
			else
				printf("%02X ", memory[curAdr]);
		}
		printf("; ");

		//Value
		for (col = 0; col < 16; col++)
		{
			curAdr = row * 16 + col;
			if (curAdr > 0xFFFFF) {
				over = TRUE;
				break;
			}
			//Range before start
			if (curAdr < start)
				printf(". ");
			//Range after end
			else if (curAdr > end)
				printf(". ");
			else {
				if ((int)memory[curAdr] >= 0x20 && (int)memory[curAdr] <= 0x7E)
					printf("%c ", memory[curAdr]);
				else 
					printf(". ");
			}
		}
		printf("\n");
	}
	if (over)
		*nextAdr = 0;
	else
		*nextAdr = end + 1;

	return TRUE;
}

//change a memory's content
int cmd_edit(int adr, int value) {
	if (!checkRange(adr)) {
		printf("Wrong Address!\n");
		return FALSE;
	}
	memory[adr] = value;

	return TRUE;
}

//changes memory to value from start adr to end adr
int cmd_fill(int start, int end, int value) {
	int rowStart, rowEnd, row, col, curAdr;

	if (end < start) {
		printf("Invalid Range!\n");
		return FALSE;
	}

	if (!(checkRange(start) && checkRange(end))) {
		printf("Invalid Range!\n");
		return FALSE;
	}
	rowStart = start / 16;
	rowEnd = end / 16;

	for (row = rowStart; row <= rowEnd; row++) {
		for (col = 0; col < 16; col++) {
			curAdr = row * 16 + col;
			//change curAdr memory to value if curAdr is not out of bound
			if (curAdr >= start && curAdr <= end)
				memory[curAdr] = value;
		}
	}

	return TRUE;
}

//resets entire memory
void cmd_reset() {
	memset(memory, 0, sizeof(char)*MEM_SIZE);
}

int hashSearch_opcode(char* mnem) {
	//Convert lowercase to uppercase
	LowerToUpper(mnem);
	int index = hashfunction(mnem);
	hptr temp = optable[index];
	//search untill the end of one optable's index
	while (temp != NULL) {
		//if the mnemonic that i'm looking for is found
		if (!strcmp(mnem, temp->mnem))
			return temp->opcode;
		//otherwise continue to the next node
		temp = temp->next;
	}
	return -1;
}

char* hashSearch_format(char* mnem) {
	//Convert lowercase to uppercase
	LowerToUpper(mnem);
	int index = hashfunction(mnem);
	hptr temp = optable[index];
	//search untill the end of one optable's index
	while (temp != NULL) {
		//if the mnemonic that i'm looking for is found
		if (!strcmp(mnem, temp->mnem))
			return temp->format;
		//otherwise continue to the next node
		temp = temp->next;
	}
	return NULL;
}

int addressingMode(char* str){
	if(str[0] == '#')
		return 1;
	else if(str[0] == '@')
		return 2;
	else
		return 3;
}

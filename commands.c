#include "commands.h"
#include "util.h"
#include "20151619.h"
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
	int argCount, num, numCount;
	int isFirst = TRUE, errorFlag = FALSE;
	int opcode,format=-1, locctr=0,i=0;
	
	intermptr intermediate;
	intermptr newinterm;

	//PASS 1
	if (fp == NULL){
		printf("File doesn't exist!\n");
		return;
	}

	free(symboltable);
	symboltable = (symptr*)malloc(sizeof(symptr)*SYM_SIZE);

	//Get 1 line at a time from file until NULL
	while(fgets(line,200,fp) != NULL){
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

		if(argCount < 0)
			printf("[OP2:%s] %s",operand2,line);
		if(strcmp(label,"")){
			int index = symfunction(label);
			printf("[label:%s] %s",label,line);
			symtab_push(&(symboltable[index]), label, locctr);
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
		printf("(%s is format %s)\n\n",operation,tempFormat);
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
				//printf("[%X]RESB operand %d\n",locctr,num);
				locctr += num;
			}
			else if(!strcmp(operation, "BYTE")){
				//Character input
				tempContent = extractContent(operand);
				if(operand[0] == 'X'){
					num = (int)strlen(tempContent);
					locctr += num;
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
				
			}
			//unknown directive
			else{
				//printf("!!ELSE!! %s\n",operation);
				continue;
			}

		}//else


		//reset
		format = -1;
		memset(operation, 0, sizeof operation);
		memset(label, 0, sizeof label);
		memset(operand, 0, sizeof operand);
		memset(operand2, 0, sizeof operand2);
	}//while

	for(int i=0;i<HASH_SIZE;i++)
		symtab_print(symboltable[i]);
	fclose(fp);

	//PASS 2

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
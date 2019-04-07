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
	char* tempContent, *fname, fname1[30], fname2[30];
	int argCount, num, numCount, base, hashindex, totalLength;
	int isFirst = TRUE, errorFlag = FALSE;
	int opcode,format=-1, locctr=0,i=0;
	
	intermptr intermediate=NULL;
	intermptr newinterm;
	symptr* tempsymtab; 

	//PASS 1
	if (fp == NULL){
		printf("File doesn't exist!\n");
		return;
	}
	
	tempsymtab = (symptr*)calloc(SYM_SIZE,sizeof(symptr)*SYM_SIZE);

	//Get 1 line at a time from file until NULL
	while(fgets(line,200,fp) != NULL && errorFlag == FALSE){
		//empty line
		if(!strcmp(line,"") || !strcmp(line,"\n"))
			continue;
		//ignore comment line
		i=0;
		while(line[i] == ' ' || line[i] =='\t')
			i++;
		if(line[i] == '.'){
			newinterm = (intermptr)calloc(1,sizeof(interm));
			strcpy(newinterm->line,line);
			newinterm->addr = -1;
			interm_push(&intermediate,newinterm);
			continue;
		}

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
			if(symtab_push(&(tempsymtab[index]), label, locctr) == -1){
				errorFlag = TRUE;
				printf("%s already exists!\n",label);
			}
		}
			

		//make new node for a line of intermediate file
		newinterm = (intermptr)calloc(1,sizeof(interm));
		newinterm->addr = locctr;
		newinterm->next = NULL;
		newinterm->argCount = argCount;

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
		strcpy(newinterm->line,line);
		strcpy(newinterm->label,label);
		interm_push(&intermediate,newinterm);

		//increase LOCCTR
		if(format != -1){
			locctr += format;
		}//if
		//directives without opcode such as RESB, RESW
		else{
			if(!strcmp(operation,"RESW")){
				num = StrToInt(operand);
				locctr += num * 3;
			}
			else if(!strcmp(operation,"RESB")){
				num = StrToInt(operand);
				locctr += num;
			}
			else if(!strcmp(operation, "BYTE")){
				//Character input
				tempContent = extractContent(operand);
				if(operand[0] == 'X'){
					num = (int)strlen(tempContent);
					locctr += ceil((float)num/2);
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
				locctr += 3;
			}
			else if(!strcmp(operation,"END")){
				//printf("[%X]END operand %d\n",locctr,num);
				totalLength = locctr;
			}
			else if(!strcmp(operation,"BASE")){
				//base = symtab_search(tempsymtab[hashindex],operand);
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
	//--------------------------------------------
	//PASS 2
	//--------------------------------------------
	if (intermediate == NULL){
		printf("No lines to assemble\n");
	}
	else{
		int offset, pc, hashindex, linenum=0;
		unsigned int obj12=0,obj3=0,obj4=0;
		char strcontent[30];

		FILE *lstfile, *objfile;
		intermptr curline;
		curline = intermediate;

		//open lstfile file
		fname = strtok(filename,".");
		strcpy(fname1,fname);
		strcpy(fname2,fname);
		strcat(fname1,".lst");
		strcat(fname2,".obj");	
		lstfile = fopen(fname1,"w+");
		objfile = fopen(fname2,"w+");

		//Start Header record
		fprintf(objfile,"H");

		while(curline != NULL && errorFlag == FALSE){
			//lines that don't produce opcodes
			curline->line[strlen(curline->line)-1] = '\0';
			fprintf(lstfile,"%04d\t",linenum);
			if(curline->addr == -1){
				fprintf(lstfile,"\t%s\n",curline->line);
				curline = curline->next;
				continue;
			}

			fprintf(lstfile,"%04X\t",curline->addr);
			fprintf(lstfile,"%-30s",curline->line);
			linenum +=5;

			if(compareString(curline->operation,"RESW","RESB") || compareString(curline->operation,"BASE",NULL)){
				fprintf(lstfile,"\n");
				curline = curline->next;
				continue;
			}
			else if(!strcmp(curline->operation,"START")){
				fprintf(lstfile,"\n");
				fprintf(objfile,"%6s",curline->label);
				fprintf(objfile,"%06X",curline->addr);
				fprintf(objfile,"%06X\n",totalLength);
				curline = curline->next;
				continue;
			}
			else if(!strcmp(curline->operation,"END")){
				//get the address of the symbol that END points
				hashindex = symfunction(curline->operand);
				offset = symtab_search(tempsymtab[hashindex],curline->operand);
				fprintf(lstfile,"\n");
				fprintf(objfile,"\nE%6X",offset);
				
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
					offset = symtab_search(tempsymtab[hashindex],strcontent);
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
							if(offset - pc >= -2048 && offset - pc <= 2047){
								//use pc relative
								obj4 = offset - pc;
								obj3 += 2;
							}
							else{
								//use base relative
								obj4 = offset - base;
								obj3 += 4;
							}
						}
					}
				}//if argCount > 1
				else{
					obj12 += 3;
				}	
			}
			//When BYTE directive is inserted
			else if(!strcmp(curline->operation, "BYTE")){
				//Extract content, removing C and X
				char *tempContent = extractContent(curline->operand);
				if(curline->operand[0] == 'X'){
					fprintf(lstfile,"%s\n",tempContent);
				}
				//Character inserted
				else if(curline->operand[0] == 'C'){
					for(int i=0; i<strlen(tempContent); i++)
						fprintf(lstfile,"%2X",(int)(tempContent[i]));
					fprintf(lstfile,"\n");
				}
				else
					fprintf(lstfile,"What is this input? %s\n",curline->operand);
				curline = curline->next;
				continue;
			}//format3,4 if
			else if(!strcmp(curline->operation, "WORD")){
				//Character input
				int temp = StrToInt(curline->operand);
				fprintf(lstfile,"%06X\n",temp);
				curline = curline->next;
				continue;
			}

			switch(curline->format){
				case 1:
					fprintf(lstfile,"%02X\n",obj12);
					break;
				case 2:
					fprintf(lstfile,"%02X%02X\n",obj12,obj3);
					break;
				case 3:
					fprintf(lstfile,"%02X%01X%03X\n",obj12,obj3,obj4%(1<<12));
					break;
				case 4:
					fprintf(lstfile,"%02X%01X%05X\n",obj12,obj3,obj4%(1<<20));
					break;
				default:
					printf("OTHER!: %s\n",curline->operation);
			}
			
			curline = curline->next;
		}//while

		fclose(lstfile);
		fclose(objfile);
	}//else

	if(errorFlag){
		printf("ERROR!!\n");
	}
	//if symbol table is alreay set, clear
	if(!newsymtable){
		for(int i = SYM_SIZE; i>=0; i--)
			free(symboltable[i]);
		free(symboltable);
		symboltable = NULL;
	}
	symboltable = tempsymtab;
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

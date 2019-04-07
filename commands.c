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
	//prints 1 line at a time until FILE ends
	while(fgets(line,100,fp) != NULL)
			printf("%s",line);
	fclose(fp);
	return;
}

int cmd_assemble(char* filename){
	FILE *fp = fopen(filename, "r");
	char line[200]={0,}, line2[200]={0,};
	char label[30], operand[30], operand2[30], operation[30], *tempFormat;
	char* tempContent, *fname, fname1[30], fname2[30];
	int argCount, num, numCount, base, hashindex, totalLength;
	int isFirst = TRUE, errorFlag = FALSE;
	int opcode,format=-1, locctr=0,linecount;
	
	intermptr intermediate=NULL;
	intermptr newinterm;
	symptr* tempsymtab; 

	//PASS 1
	if (fp == NULL){
		printf("File doesn't exist!\n");
		return TRUE;
	}
	
	tempsymtab = (symptr*)calloc(SYM_SIZE,sizeof(symptr)*SYM_SIZE);

	linecount = 0;
	//Get 1 line at a time from file until NULL
	while(fgets(line,200,fp) != NULL && errorFlag == FALSE){
		int i=0;
		//count line for error
		linecount++;
		//empty line
		if(!strcmp(line,"") || !strcmp(line,"\n"))
			continue;
		//ignore comment line
		i=0;
		while(line[i] == ' ' || line[i] =='\t')
			i++;
		//if it's a comment line
		if(line[i] == '.'){
			newinterm = (intermptr)calloc(1,sizeof(interm));
			strcpy(newinterm->line,line);
			newinterm->addr = -1;
			interm_push(&intermediate,newinterm);
			continue;
		}
		//seperates a line by label, directive and operands
		argCount = asmSeparater(line,label,operation,operand,operand2);

		//checks if the first line is START
		if(isFirst){
			//sets initial location counter with the operand
			if(!strcmp(operation,"START"))
				locctr = StrToHex(operand); 
			//if start address wasn't given, set to 0
			else
				locctr = 0;
			isFirst = FALSE;
		}

		//if there is a symbol
		if(strcmp(label,"")){
			//get the index in the symbol table
			int index = symfunction(label);
			//insert into that index of symbol table
			if(symtab_push(&(tempsymtab[index]), label, locctr) == TRUE){
				printf("ERROR at line %d\n",linecount);
				errorFlag = TRUE;
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
			//extract from index 1
			tempFormat = hashSearch_format(&(operation[1]));
			opcode = hashSearch_opcode(&(operation[1]));
			strcpy(newinterm->operation,&(operation[1]));
			if(tempFormat == NULL){
				printf("ERROR at line %d\n",linecount);
				errorFlag = TRUE;
				continue;
			}
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
				//remove indicator X and C
				tempContent = extractContent(operand);
				//if the input is in Hexadecimal
				if(operand[0] == 'X'){
					num = (int)strlen(tempContent);
					locctr += ceil((float)num/2);
				}
				//if the input is in Character
				else if(operand[0] == 'C'){
					locctr += strlen(tempContent);
				}
				//other than these 2, error
				else{
					printf("ERROR at line %d\n",linecount);
					errorFlag = TRUE;
				}
			}
			else if(!strcmp(operation, "WORD")){
				num = StrToInt(operand);	
				locctr += 3;
			}
			else if(!strcmp(operation,"END")){
				totalLength = locctr;
			}
			else if(compareString(operation,"START","BASE")){
				opcode = 0;
			}
			//unknown directive
			else{
				printf("ERROR at line %d\n",linecount);
				errorFlag = TRUE;
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
	fclose(fp);

	//stop if there was an error
	if(errorFlag)
		return TRUE;

	//--------------------------------------------
	//PASS 2
	//--------------------------------------------
	if (intermediate == NULL){
		printf("No lines to assemble\n");
		return TRUE;
	}
	else{
		int offset, pc, hashindex, newlineadr;
		int colcount=0,colcount2=0, linenum=0, newlineFlag=FALSE;
		int *modification = (int*)malloc(sizeof(int)*300);
		int modfIdx=0,modfSize=300;
		unsigned int obj12=0,obj3=0,obj4=0;
		char strcontent[30];

		FILE *lstfile, *objfile;
		intermptr curline;
		curline = intermediate;

		//open list file and object file
		fname = strtok(filename,".");
		strcpy(fname1,fname);
		strcpy(fname2,fname);
		strcat(fname1,".lst");
		strcat(fname2,".obj");	
		lstfile = fopen(fname1,"w+");
		objfile = fopen(fname2,"w+");

		
		//reset values;
		colcount = 9;
		colcount2 = 0;
		newlineadr = -1;
		memset(line,0,sizeof(line));

		//if first line isn't Start
		if(strcmp(curline->operation,"START"))
			fprintf(objfile,"H      000000%06X\n",totalLength);

		linecount = 0;
		while(curline != NULL && errorFlag == FALSE){
			char objstr[3][20];
			linecount++;
			//lines that don't produce opcodes
			curline->line[strlen(curline->line)-1] = '\0';
			//print line number
			fprintf(lstfile,"%04d\t",linenum);
			linenum +=5;
			//if current line of intermediate file is a comment line
			if(curline->addr == -1){
				fprintf(lstfile,"\t%s\n",curline->line);
				curline = curline->next;
				continue;
			}

			//print location counter if it's not END
			if(strcmp(curline->operation,"END"))
				fprintf(lstfile,"%04X\t",curline->addr);
			else
				fprintf(lstfile,"    \t");
			//print assembly code
			fprintf(lstfile,"%-30s",curline->line);
			

			//if current line is RESW or RESB, start new line
			if(compareString(curline->operation,"RESW","RESB")){
				fprintf(lstfile,"\n");
				newlineFlag = TRUE;
				curline = curline->next;
				continue;
			}
			//if current line is BASE, move to the next line
			else if(!strcmp(curline->operation,"BASE")){
				fprintf(lstfile,"\n");
				curline = curline->next;
				continue;
			}
			//if current line is START
			else if(!strcmp(curline->operation,"START")){
				fprintf(lstfile,"\n");
				//Start Header record
				fprintf(objfile,"H");
				fprintf(objfile,"%-6s",curline->label);
				fprintf(objfile,"%06X",curline->addr);
				fprintf(objfile,"%06X\n",totalLength-curline->addr);
				//start a new Text Record
				fprintf(objfile,"T%06X",curline->addr);
				curline = curline->next;
				continue;
			}
			else if(!strcmp(curline->operation,"END")){
				fprintf(lstfile,"\n");
				//print last line TEXT LINE of obj file
				if(strcmp(line,""))
					fprintf(objfile,"%02X%s",(int)(strlen(line))/2,line);
				if(strcmp(line2,""))
					fprintf(objfile,"\nT%06X%s",newlineadr,line2);
				//modification record
				for(int k=0;k<modfIdx;k++)
					fprintf(objfile,"\nM%06X05",modification[k]);
				//get the address of the symbol that END points
				hashindex = symfunction(curline->operand);
				offset = symtab_search(tempsymtab[hashindex],curline->operand);
				fprintf(objfile,"\nE%06X\n",offset);
				
				curline = curline->next;
				continue;
			}

			//initialize variables
			obj12 = obj3 = obj4 = 0;
			for(int i=0;i<3;i++)
				memset(objstr[i],0,sizeof(objstr[i]));

			//intialize new line
			if(newlineFlag == TRUE){
				//if there's anything in line
				if(strcmp(line,"")){
					fprintf(objfile,"%02X%s\n",(int)(strlen(line))/2,line);
					memset(line,0,sizeof(line));
				}
				//line2 is empty
				if(!strcmp(line2,"")){
					fprintf(objfile,"T%06X",curline->addr);
				}
				//if line2 isn't empty
				else{
					fprintf(objfile,"T%06X",newlineadr);
					strcpy(line,line2);
					memset(line2,0,sizeof(line2));
				}
				//reset values for new line
				colcount = 9 + colcount2;
				colcount2 = 0;
				newlineadr = -1;
				newlineFlag = FALSE;
			}
			

			//first 2 digits of object code
			obj12 = hashSearch_opcode(curline->operation);

			//format 2
			if(curline->format == 2){
				//if command is SVC
				if(!strcmp(curline->operand,"SVC")){
					int temp = StrToHex(curline->operand);
					obj3 = insertHexAt(obj3,temp,1);
				}
				else{
					int temp = registerNum(curline->operand);
					//if operand isn't a register
					if(temp == -1){
						printf("ERROR at line %d\n",linecount);
						errorFlag = TRUE;
						continue;
					}
					obj3 = insertHexAt(obj3,temp,1);
					//If second operand exists
					if(curline->argCount < 0){
						//if directive is SHIFTL or SHIFTR, 2nd operand is an integer
						if(compareString(curline->operand2,"SHIFTL","SHIFTR")){
							temp = StrToHex(curline->operand2);
							obj3 = insertHexAt(obj3,temp,0);
						}
						else{
							temp = registerNum(curline->operand2);
							//if operand isn't a register
							if(temp == -1){
								printf("ERROR at line %d\n",linecount);
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
							printf("ERROR at line %d\n",linecount);
							errorFlag = TRUE;
							continue;
						}
					}
					else{
						//if format4, direct addressing
						if(curline->format == 4){
							//if it is not immediate addressing
							if(curline->operand[0] != '#'){
								//add to modification record
								modification[modfIdx++] = curline->addr + 1;
								if(modfIdx == modfSize - 1){
									modfSize *= 2;
									modification = (int*)realloc(modification,sizeof(int)*modfSize);
								}
							}
							
							obj4 = offset;
						}
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
					//write content to file
					fprintf(lstfile,"%s\n",tempContent);
					//if this line can fit into the current line of object file
					if(colcount + strlen(tempContent) <= MAX_TEXT_LINE){
						colcount += strlen(tempContent);
						strcat(line,tempContent);
					}
					//if this line needs to be on the next line of object file
					else{
						newlineFlag = TRUE;
						colcount2 = strlen(tempContent);
						newlineadr = curline->addr;
						strcat(line2,tempContent);
					}
				}
				//Character inserted
				else if(curline->operand[0] == 'C'){
					char convstr[10];
					char tempstr[10];
					//convert each character to corresponding ASCII in hex and write to file
					for(int i=0; i<strlen(tempContent); i++){
						fprintf(lstfile,"%2X",(int)(tempContent[i]));
						sprintf(convstr,"%2X",(int)(tempContent[i]));
						strcat(tempstr,convstr);
					}
					fprintf(lstfile,"\n");
					//if this line can fit into the current line of object file
					if(colcount + strlen(tempstr) <= MAX_TEXT_LINE){
						colcount += strlen(tempstr);
						strcat(line,tempstr);
					}
					//if this line needs to be on the next line of object file
					else{
						newlineFlag = TRUE;
						colcount2 = strlen(tempstr);
						newlineadr = curline->addr;
						strcat(line2,tempstr);
					}
				}
				else
					fprintf(lstfile,"%s\n",curline->operand);
				curline = curline->next;
				continue;
			}//format3,4 if
			else if(!strcmp(curline->operation, "WORD")){
				char tempstr[10];
				int temp = StrToInt(curline->operand);
				sprintf(tempstr,"%06X",temp);
				fprintf(lstfile,"%06X\n",temp);
				//if this line can fit into the current line of object file
				if(colcount + strlen(tempstr) <= MAX_TEXT_LINE){
					colcount += strlen(tempstr);
					strcat(line,tempstr);
				}
				//if this line needs to be on the next line of object file
				else{
					newlineFlag = TRUE;
					colcount2 = strlen(tempstr);
					newlineadr = curline->addr;
					strcat(line2,tempstr);
				}
				curline = curline->next;
				continue;
			}
			//make first 2 Hexadecimal
			sprintf(objstr[0],"%02X",obj12);
			//make the rest of object codes
			switch(curline->format){
				//format 1
				case 1:
					fprintf(lstfile,"%02X\n",obj12);
					break;
				//format 2
				case 2:
					fprintf(lstfile,"%02X%02X\n",obj12,obj3);
					sprintf(objstr[1],"%02X",obj3);
					break;
				//format 3
				case 3:
					fprintf(lstfile,"%02X%01X%03X\n",obj12,obj3,obj4%(1<<12));
					sprintf(objstr[1],"%01X",obj3);
					sprintf(objstr[2],"%03X",obj4%(1<<12));
					break;
				//format 4
				case 4:
					fprintf(lstfile,"%02X%01X%05X\n",obj12,obj3,obj4%(1<<20));
					sprintf(objstr[1],"%01X",obj3);
					sprintf(objstr[2],"%05X",obj4%(1<<20));
					break;
			}
			//if this line can fit into the current line of object file
			if(colcount + curline->format*2 <= MAX_TEXT_LINE){
				colcount += curline->format*2;
				for(int k=0;k<curline->format && k<3;k++)
					strcat(line,objstr[k]);
			}
			//if this line needs to be on the next line of object file
			else{
				newlineFlag = TRUE;
				colcount2 = curline->format*2;
				newlineadr = curline->addr;
				for(int k=0;k<curline->format && k<4;k++)
					strcat(line2,objstr[k]);
			}

			curline = curline->next;
		}//while
		fclose(lstfile);
		fclose(objfile);
	}//else

	if(errorFlag){
		remove(fname1);
		remove(fname2);
		return TRUE;
	}
	//if symbol table is alreay set, clear
	if(!newsymtable){
		for(int i = SYM_SIZE; i>=0; i--)
			free(symboltable[i]);
		free(symboltable);
		symboltable = NULL;
	}

	printf("\noutput file: [%s], [%s]\n",fname1,fname2);

	//successfully assembled. Set global symbol table to current file's symbol table
	symboltable = tempsymtab;
	newsymtable = FALSE;

	return FALSE;
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

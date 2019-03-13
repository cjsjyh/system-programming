#include "commands.h"
#include "main.h"

void cmd_help() {
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start,end]\n");
	printf("e[dit] address, value\nf[ill] start, end, value\nreset\n");
	printf("opcode mnemonic\nopcodelist\n");
	return;
}

void cmd_dir() {
	DIR *dir;
	struct dirent *file;
	struct stat info;

	if ((dir = opendir("./")) != NULL) {
		/* print all the files and directories within directory */
		while ((file = readdir(dir)) != NULL) {
			stat(file->d_name, &info);

			if (info.st_mode == EXECUTABLE)
				printf("%s*\n", file->d_name);
			else if (info.st_mode == DIRECTORY)
				printf("%s/\n", file->d_name);
			else
				printf("%s\n", file->d_name);

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

int cmd_dump(int start, int end, int* nextAdr) {
	int row, col, over = FALSE;
	int rowStart, rowEnd, curAdr;

	if (end == INT_MIN) {
		if (start == INT_MIN)
			start = *nextAdr;
		end = start + 159;
	}
	else{
		if (checkRange(end) == FALSE){
			printf("Invalid Range\n");
			return FALSE;
		}
		
	}

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

int cmd_edit(int adr, int value) {
	if (!checkRange(adr)) {
		printf("Wrong Address!\n");
		return FALSE;
	}
	memory[adr] = value;

	return TRUE;
}

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
			if (curAdr >= start && curAdr <= end)
				memory[curAdr] = value;
		}
	}

	return TRUE;
}

void cmd_reset() {
	memset(memory, 0, sizeof(char)*MEM_SIZE);
}

int cmd_hashlistSearch(char* mnem) {
	//Convert lowercase to uppercase
	LowerToUpper(mnem);
	int index = hashfunction(mnem);
	hptr temp = optable[index];
	while (temp != NULL) {
		if (!strcmp(mnem, temp->mnem))
			return temp->opcode;
		temp = temp->next;
	}
	return -1;
}

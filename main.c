#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **memory;
void cmd_dump(int, int);

int main()
{
	int q_flag = 1;
	char command[30],arg1[20],arg2[20],arg3[20];
	
	
	memory=(char**)malloc(sizeof(char*)*65536);
	for (int i=0; i<65536; i++)
	{
		memory[i] = (char*)malloc(sizeof(char)*16);
		memset(memory[i],16,sizeof(char)*16);
	}
	
	while(q_flag)
	{
		printf("sicsim>");
		scanf("%s",command);
		//DUMP!
		if (!strcmp(command,"du") || !strcmp(command,"dump"))
			cmd_dump(0,2);
	}
}

void cmd_dump(int start, int end)
{
	for(int i=0; i<10; i++){
		printf("%05x  ",start + 16*i);
		for(int j=0; j<16; j++)
			printf("%02x ",memory[i][j]);
		
		for(int j=0; j<16; j++)
		{
			if (memory[i][j] >= 20 && memory[i][j] <= 0x7E){
				printf("%c ",memory[i][j]);
			} else{
				printf(". ");
			}
		}
		printf("\n");
	}
}

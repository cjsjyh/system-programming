#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int q_flag = 1;
	char command[30],arg1[20],arg2[20],arg3[20];
	char **memory;
	
	memory=(char**)malloc(sizeof(char*)*65536);
	for (int i=0; i<65536; i++)
	{
		memory[i] = (char*)malloc(sizeof(char)*16);
		memset(memory[i],0,sizeof(char)*16);
	}
	
	while(q_flag)
	{
		printf("sicsim>");
		scanf("%s",command);
		//DUMP!
		if (!strcmp(command,"du") || !strcmp(command,"dump"))
			for(int i=0; i<10; i++){
				for(int j=0; j<16; j++)
					printf("%02x ",memory[i][j]);
				printf("\n");
	}		}
}

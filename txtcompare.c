#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
	char fname1[30];
	char fname2[30];

	char temp1[50];
	char temp2[50];

	int line =0;

	scanf("%s",fname1);
	scanf("%s",fname2);

	FILE* fp = fopen(fname1,"r");
	FILE* fp2 = fopen(fname2, "r");

	if(fp == NULL || fp2 == NULL){
		printf("Error opening File\n");
		return 0;
	}

	while(fscanf(fp,"%s",temp1) != EOF && fscanf(fp2,"%s",temp2) != EOF){
		if(!strcmp(temp1,temp2)){
			printf("[%d]OK\n",line);
		}
		else{
			printf("[%d]Diff! [%s] [%s]\n",line,temp1,temp2);
		}
		line++;
	}

	return 0;
}

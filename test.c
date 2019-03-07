#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	//char b[] = "test1 test2 test3";
	char b[30];
	char command[30];
	int a,c,d;
	
	fgets(b,sizeof b,stdin);	


	printf("%d\n",sscanf(b,"%s %d %d %d",command,&a,&c,&d));
}

















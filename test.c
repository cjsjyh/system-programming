#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	//char b[] = "test1 test2 test3";
	char b[30];
	char b1[5], b2[5], b3[5], b4[5];
	int c1,c2,c3;
	
	fgets(b,sizeof b,stdin);	


	printf("%d\n",sscanf(b,"%s %d, %d",b1,&c1,&c2));
	printf("%d\n",sscanf(b,"%s %s %s %s",b1,b2,b3,b4));
	printf("0: %s 1: %s 2: %s 3: %s\n",b1,b2,b3,b4);
}

















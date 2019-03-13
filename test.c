#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	//char b[] = "test1 test2 test3";
	char b = 'c';
	char c[100],d[30];
	int intt;

	fgets(c, sizeof c,stdin);
	printf("%d\n",sscanf(c,"%s %x",d,&intt));
	printf("%s %d\n",d,(int)strlen(d));

}

















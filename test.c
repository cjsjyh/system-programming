#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {

	char *a = "c'test'";
	char b[30];

	strncpy(b,&(a[2]),strlen(a)-3);
	b[4] = 0;
	printf("%s\n",b);

	return 0;
}

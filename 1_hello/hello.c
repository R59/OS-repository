#include <stdio.h>

int main()
{
	//fclose(stdout);
	if(printf("Hello, world!\n") == EOF)
		return 1;
	return 0;
}
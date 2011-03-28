#include <stdio.h>

#define CPU 1000000000
#define N 1

int main()
{
	int i, j, S;
	for(i=0; i<CPU; ++i)
		for(j=0; j<N; ++j)
			S = 0;
	puts("End");
	return 0;
}

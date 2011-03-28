#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CPU 1000000000
#define SIZE (1024*1024)
#define maxX (1<<20)

int main()
{
	char *tmp = NULL;
	tmp = malloc(SIZE*sizeof(char));
	if(tmp==NULL)
		return 1;

	puts("size of row\trows\tcolumns");
	int X;
	for(X=1; X<=maxX; X<<=1)
	{
		char (*A)[X] = (char (*)[X])tmp;
		int Y = SIZE/X;

		int S, m, x, y;
		time_t t1 = time(NULL);

		for(m=0; m<CPU/SIZE; ++m)
			for(y=0; y<Y; ++y)
				for(x=0; x<X; ++x)
					S += A[y][x];

		time_t t2 = time(NULL);

		for(m=0; m<CPU/SIZE; ++m)
			for(x=0; x<X; ++x)
				for(y=0; y<Y; ++y)
					S += A[y][x];

		time_t t3 = time(NULL);

		printf("%d\t\t%d\t%d\n", X, (int)(t2-t1), (int)(t3-t2));
	}

	free(tmp);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CPU 1000000000
#define X 100
#define Y 2500
#define M CPU/(X*Y)

int main()
{
	int **A = NULL, S, k, i, j;
	A = (int **)malloc(Y*sizeof(int *));
	if(A==NULL)
		return 5;

	for(k=0; k<Y; ++k)
	{
		A[k] = (int *)malloc(X*sizeof(int));
		if(A[k]==NULL)
			return k+10;
	}

	time_t t1 = time(NULL);

	for(k=0; k<M; ++k)
		for(i=0; i<Y; ++i)
			for(j=0; j<X; ++j)
				S += A[i][j];

	time_t t2 = time(NULL);

	for(k=0; k<M; ++k)
		for(i=0; i<X; ++i)
			for(j=0; j<Y; ++j)
				S += A[j][i];

	time_t t3 = time(NULL);

	for(k=0; k<Y; ++k)
		free(A[k]);
	free(A);
/*
	int A[Y][X], S;
	int i, j, k;

	time_t t1 = time(NULL);

	for(k=0; k<M; ++k)
		for(i=0; i<Y; ++i)
			for(j=0; j<X; ++j)
				S = A[i][j];

	time_t t2 = time(NULL);

	for(k=0; k<M; ++k)
		for(i=0; i<X; ++i)
			for(j=0; j<Y; ++j)
				S = A[j][i];

	time_t t3 = time(NULL);
*/
	printf("by rows:\t%d\nby columns:\t%d\n", (int)(t2-t1), (int)(t3-t2));
	return 0;
}

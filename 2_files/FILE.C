#include <stdio.h>
#include <stdlib.h>

#define STARTSIZE 16

struct massiv
{
	int nowsize, maxsize, *data;
};

int read(FILE *file, massiv &X);
int error(unsigned int x);
void sort(massiv &X);

int main(int argc, char **argv)
{
	int err = 0;
	massiv A = {0, STARTSIZE, (int *)malloc(STARTSIZE*sizeof(int))};

	if(argc<3)
		err = 1;
	else if(A.data == NULL)
		err = 2;
	else
	{
		FILE *out = fopen(argv[argc-1],"wt");
		if(out == NULL)
			err = 3;
		else
		{
			for(int i=1; i<argc-1 && !err; ++i)
			{
				FILE *in = fopen(argv[i], "rt");
				if(in == NULL)
					err = 4;
				else
				{
					err = read(in,A);

					if(fclose(in) == EOF && !err)
						err = 5;
				}
			}

			if(!err)
			{
				sort(A);

				for(int i=0; i<A.nowsize; ++i)
					if(fprintf(out,"%d\n",A.data[i]) == EOF)
					{
						err = 8;
						break;
					}
			}

			if(fclose(out) == EOF && !err)
				err = 6;
		}

		free(A.data);
	}

	return error(err);
}

int read(FILE *file, massiv &X)
{
	int c, y;
	while((c=fscanf(file,"%d",&y))!=EOF && c)
	{
		if(X.nowsize == X.maxsize)
		{
			X.data = (int *)realloc(X.data, X.maxsize<<=1);
			if(X.data == NULL)
				return 2;
		}
		X.data[X.nowsize++] = y;
	}
	return c==0 ? 7:0;
}

void sort(massiv &X)
{
	for(int i=0; i<X.nowsize-1; ++i)
	{
		int	min = i;
		for(int j=i+1; j<X.nowsize; ++j)
			if(X.data[j]<X.data[min])
				min = j;

		int y = X.data[i];
		X.data[i] = X.data[min];
		X.data[min] = y;
	}
}

int error(unsigned int err)
{
	if(!err)
		return 0;

	char *message[] = {
						"Some error",
						"Too few parameters",
						"Not anough memory",
						"Can't open output file",
						"Can't open input file",
						"Can't close input file",
						"Can't close output file",
						"Read error",
						"Write error",
					};

	if(err>sizeof(message)/sizeof(char *))
		err = 0;

	return puts(message[err])==EOF ? err:0;
}

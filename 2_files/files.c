#include <stdio.h>
#include <stdlib.h>

#define STARTSIZE 16

struct massiv
{
	int nowsize, maxsize, *data;
};

int proga(int argc, char **argv, struct massiv *X);
int read_files(int files_count, char **files, struct massiv *X);
int read(FILE *file, struct massiv *X);
void sort(struct massiv *X);
void show_error(unsigned int error);

int main(int argc, char **argv)
{
	int err = 0;

	if(argc<3)
		err = 1;
	else
	{
		struct massiv A = {0, STARTSIZE, NULL};
		A.data = malloc(STARTSIZE*sizeof(int));
		if(A.data == NULL)
			err = 2;
		else
		{
			err = proga(argc, argv, &A);
			free(A.data);
		}
	}

	if(err)
		show_error(err);

	return err;
}

int proga(int argc, char **argv, struct massiv *X)
{
	FILE *out = fopen(argv[argc-1],"wt");
	if(out == NULL)
		return 3;

	int err = read_files(argc-2, argv+1, X);
	if(!err)
	{
		sort(X);
		int i;
		for(i=0; i<X->nowsize; ++i)
			if(fprintf(out, "%d\n", X->data[i]) == EOF)
				err = 8;
	}

	if(fclose(out) == EOF && !err)
		return 6;

	return err;
}

int read_files(int files_count, char **files, struct massiv *X)
{
	int i;
	for(i=0; i<files_count; ++i)
	{
		FILE *in = fopen(files[i], "rt");
		if(in == NULL)
			return 4;

		int err = read(in, X);

		if(fclose(in) == EOF && !err)
			return 5;

		if(err)
			return err;
	}

	return 0;
}

int read(FILE *file, struct massiv *X)
{
	int code, num;
	while((code=fscanf(file,"%d",&num))!=EOF)
	{
		if(!code)
			return 7;

		if(X->nowsize == X->maxsize)
		{
			X->data = realloc(X->data, X->maxsize<<=1);
			if(X->data == NULL)
				return 2;
		}
		X->data[X->nowsize++] = num;
	}

	return 0;
}

void sort(struct massiv *X)
{
	int i;
	for(i=0; i<X->nowsize-1; ++i)
	{
		int	min = i, j;
		for(j=i+1; j<X->nowsize; ++j)
			if(X->data[j]<X->data[min])
				min = j;

		int tmp = X->data[i];
		X->data[i] = X->data[min];
		X->data[min] = tmp;
	}
}

void show_error(unsigned int error)
{
	char *message[] = {
						"Unknown error",
						"Too few parameters",
						"Not enough memory",
						"Can't open output file",
						"Can't open input file",
						"Can't close input file",
						"Can't close output file",
						"Read error",
						"Write error",
					};

	if(error > sizeof(message)/sizeof(char *))
		error = 0;

	puts(message[error]);
}

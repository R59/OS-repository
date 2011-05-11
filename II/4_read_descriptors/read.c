#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

#define BUFSIZE 10

int main(int argc, char**argv)
{
	if(argc<2)
	{
		puts("\n\tUsage: program desc1 [desc2 ...]\n");
		return 1;
	}

	int *numbers = malloc((argc-1)*sizeof(int));
	if(numbers==NULL)
	{
		puts("Error: not enough memory");
		return 2;
	}


	fd_set set;
	FD_ZERO(&set);
	int max = 0;
	for(int i=0; i<argc-1; ++i)
	{
		if(!sscanf(argv[i+1], "%d", numbers+i))
		{
			puts("Error: argument should be integer number");
			free(numbers);
			return 3;
		}
		FD_SET(numbers[i], &set);
		if(numbers[i] > max)
			max = numbers[i];
	}

	for(fd_set tmp=set; 1; set=tmp)
	{
		if(select(max+1, &set,NULL,NULL, NULL) == -1)
		{
			puts("Error: select");
			free(numbers);
			return 4;
		}

		char buf[BUFSIZE];
		int readed;
		for(int i=0; i<argc-1; ++i)
			if(FD_ISSET(numbers[i], &set))
				while( (readed=read(numbers[i],buf,BUFSIZE))>0 )
				{
					write(1, buf, readed);
					puts("\n----------");
				}
	}

	free(numbers);
	return 0;
}

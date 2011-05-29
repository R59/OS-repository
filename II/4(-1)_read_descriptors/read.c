#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
//#include <sys/ioctl.h>
#include <sys/fcntl.h>
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
		if(sscanf(argv[i+1], "%d", numbers+i)<=0)
		{
			puts("Error: argument should be integer number");
			free(numbers);
			return 3;
		}

		int flags = fcntl(numbers[i], F_GETFL);
		if(flags==-1 || -1==fcntl(numbers[i], F_SETFL, flags|O_NONBLOCK))
		{
			puts("Error: fcntl");
			free(numbers);
			return 4;
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
			//{
			//	int len;
			//	while(ioctl(numbers[i], I_NREAD, &len)>0)
			//	{
			//		if(len>BUFSIZE)
			//			len = BUFSIZE;
			//		while( (readed=read(numbers[i],buf,len))>0 )
					while( (readed=read(numbers[i],buf,BUFSIZE))>0 )	//
					{
						write(1, buf, readed);
						puts("\n----------");
					}
			//	}
			//}
	}

	free(numbers);
	return 0;
}

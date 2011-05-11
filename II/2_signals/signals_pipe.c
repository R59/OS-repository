#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define EXIT_SIG 20

int pair[2];

void hello_signal(int number)
{
	write(pair[1], &number, sizeof(number));
}

int main()
{
	if(pipe(pair)==-1)
	{
		puts("Can't create pipe");
		return 1;
	}

	struct sigaction S;
	S.sa_handler = hello_signal;
	S.sa_flags = 0;
	if(sigfillset(&S.sa_mask)==-1)
	{
		puts("Can't set sa_mask");
		return 2;
	}

	int i;
	for(i=1; i<32; ++i)
		if(i!=SIGKILL && i!=SIGSTOP && sigaction(i, &S, NULL))
		{
			printf("Can't set signal %d\n", i);
			return 3;
		}

	int number;
	extern int errno;
	do
	{
		int readed = read(pair[0], &number, sizeof(number));
		if(!readed || (readed==-1 && errno==EINTR))
			number = -1; // number!=EXIT_SIG
		else if(readed!=-1)
			printf("Hello, signal %d!\n", number);
		else
		{
			puts("read error");
			return 4;
		}
	}
	while(number != EXIT_SIG);

	puts("Exit");
	return 0;
}

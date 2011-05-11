#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MY_SIG_EXIT 20

int EXIT = 0;

void hello_signal(int number);
void writeIntDec(int x);

int main()
{
	struct sigaction S;
	S.sa_handler = hello_signal;
	S.sa_flags = 0;
	if(sigfillset(&S.sa_mask)==-1)
	{
		puts("Can't set sa_mask");
		return 1;
	}

	int i;
	for(i=1; i<32; ++i)
		if(i!=SIGKILL && i!=SIGSTOP && sigaction(i, &S, NULL))
		{
			printf("Can't set signal %d\n", i);
			return 2;
		}

	sigset_t exitmask;
	if(sigfillset(&exitmask)==-1 || sigdelset(&exitmask, MY_SIG_EXIT))
	{
		printf("Can't set exitmask");
		return 3;
	}

	while(1)
		;

	//puts("Exit");
	return 0;
}

void hello_signal(int number)
{
	write(1, "Hello, signal ", sizeof("Hello, signal "));
	writeIntDec(number);
	write(1, "\n", 1);
//	printf("Hello, signal %d!\n", number);
	if(number==MY_SIG_EXIT)
		exit(0);
}

void writeIntDec(int x)
{
	#define maxlen 9

	char ch;
	if(x==0)
	{
		ch = '0';
		write(1,&ch,1);
		return;
	}

	if(x<0)
	{
		ch = '-';
		write(1,&ch,1);
		x = -x;
	}

	char str[maxlen];
	int i=maxlen;
	do
	{
		str[--i] = '0' + (x%10);
	}
	while(x/=10);

	write(1, str+i, maxlen-i);
}

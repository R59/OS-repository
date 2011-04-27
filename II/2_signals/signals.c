#include <stdio.h>
#include <signal.h>

int EXIT = 0;

void hello_signal(int number);

int main()
{
	struct sigaction S;
	S.sa_handler = hello_signal;
	S.sa_flags = 0;
	if(sigfillset(&S.sa_mask))
	{
		puts("Mask error");
		return 1;
	}

	int i;
	for(i=1; i<32; ++i)
		if(i!=9 && i!=19 && sigaction(i, &S, NULL))
		{
			printf("can't set signal %d\n", i);
			return 2;
		}

	while(!EXIT)
		;

	puts("Exit");
	return 0;
}

void hello_signal(int number)
{
	if(EXIT)
		return;
	printf("Hello, signal %d!\n", number);
	if(number==20)
		EXIT=1;
}


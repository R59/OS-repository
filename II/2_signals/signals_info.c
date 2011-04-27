#include <stdio.h>
#include <signal.h>

int EXIT = 0;

void hello_signal(int number, siginfo_t *info, void *something);

int main()
{
	struct sigaction S;
	S.sa_sigaction = hello_signal;
	S.sa_flags = SA_SIGINFO;
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

void hello_signal(int number, siginfo_t *info, void *something)
{
	if(EXIT)
		return;
	printf("Hello, signal %d!\n\tmy pid = %d\n\tmy guid = %d\n\tsigno = %d\n\tcode = %d\n\tpid = %d\n\tuid = %d\n", \
				number, getpid(), getgid(), info->si_signo, info->si_code, info->si_pid, info->si_uid);
	if(number==20)
		EXIT=1;
}


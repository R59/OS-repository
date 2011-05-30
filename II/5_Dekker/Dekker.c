#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define BIG_NUMBER 10000000

#define mfence __asm__("mfence");

volatile int want[2] = {0, 0};
volatile int turn = 0;

volatile int inside[2] = {0, 0};
volatile int counter = 0;

void *work(void *x);
void shared_section(int name);
void printlog(char *msg);

int main()
{
	pthread_t threads[2];
	pthread_create(&threads[0], NULL, work, (void *)0);
	pthread_create(&threads[1], NULL, work, (void *)1);

	while(counter<100)
		;

	return 0;
}

void *work(void *x)
{
	int me = (int)x;

	while(1)
	{
		mfence
		want[me] = 1;
		mfence
		while(want[me^1])
		{
			mfence
			if(turn != me)
			{
				mfence
				want[me] = 0;
				mfence
				while(turn != me)
					;
				mfence
				want[me] = 1;
				mfence
			}
		}

		shared_section(me);
		mfence
		want[me] = 0;
		mfence
		turn = me^1;
		mfence
	}
}

void shared_section(int name)
{
	inside[name] = 1;
	char s[] = "0";
	s[0] += name;
	printlog(s);
	if(inside[name^1])
		printlog("Hey, look! Somebody there!\n");

	++counter;
	for(int i=0; i<BIG_NUMBER; ++i)
		;

	inside[name] = 0;
}

void printlog(char *msg)
{
	write(1, msg, strlen(msg));	// need check error
	write(1, "\n", 1);			// need check error
}

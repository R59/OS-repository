#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

#define BIG_NUMBER 10000000

#define mfence __asm__("mfence");

volatile int want[2] = {0, 0};
volatile int turn = 0;

volatile int counter = 0;

void *work(void *x);

int main()
{
	pthread_t threads[2];
	for(int i=0; i<2; ++i)
	{
		if(pthread_create(&threads[i], NULL, work, (void *)i))
		{
			printf("Can't create thread %d", i);
			return 1;
		}
		
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(i, &mask);
		if(pthread_setaffinity_np(threads[i], sizeof(mask), &mask))
		{
			printf("Can't set affinity cpu=%d\n", i);
			return 2;
		}
	}

	for(int i=0; i<2; ++i)
		if(pthread_join(threads[i], NULL))
			printf("Error: join %d\n", i);

	printf("counter = %d\n", counter);
	return 0;
}

void *work(void *x)
{
	int me = (int)x;

	for(int i=0; i<BIG_NUMBER; ++i)
	{
		want[me] = 1;
		mfence
		while(want[me^1])
		{
			if(turn != me)
			{
				want[me] = 0;
				while(turn != me)
					;
				want[me] = 1;
			}
		}

		++counter;
		want[me] = 0;
		turn = me^1;
	}
}

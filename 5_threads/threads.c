#include <stdio.h>
#include <time.h>
#include <windows.h>

#define CPU 1000000000
#define N 1
#define SIZE 2	// 1000

int A[SIZE];

DWORD_PTR findBit(DWORD_PTR mask, DWORD_PTR from);
DWORD WINAPI inc_0();
DWORD WINAPI inc_1();

int main()
{
	HANDLE threads[2];
	void *func[] = {&inc_0, &inc_1};

	DWORD_PTR procAffMask, sysAffMask;
	if( !GetProcessAffinityMask(GetCurrentProcess(), &procAffMask, &sysAffMask) )
	{
		puts("Error: get mask");
		return 1;
	}

	int i;
	DWORD_PTR from = 1;
	for(i=0; i<2; ++i)
	{
		threads[i] = CreateThread(NULL, 0, func[i], NULL, CREATE_SUSPENDED, NULL);
		if(threads[i] == NULL)
		{
			puts("Error: create");
			return 2;
		}

		DWORD_PTR mask = findBit(procAffMask, from);
		if(!mask)
		{
			puts("Not enough logical processors");
			return 3;
		}

		if( !SetThreadAffinityMask(threads[i], mask) )
		{
			puts("Error: set affinity");
			return 4;
		}
		from = mask<<1;
	}

	ResumeThread(threads[0]);
	ResumeThread(threads[1]);
	time_t t1 = time(NULL);
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);
	time_t t2 = time(NULL);
	CloseHandle(threads[0]);
	CloseHandle(threads[1]);

	printf("time: %d sec\n", (int)(t2-t1));
	return 0;
}

DWORD_PTR findBit(DWORD_PTR mask, DWORD_PTR from)
{
	for( ; from; from<<=1)
		if(mask&from)
			return from;
	return 0;
}

DWORD WINAPI inc_0()
{
	int i;
	for(i=0; i<CPU; ++i)
	{
		A[0] = 0;
		while(A[0]++<N)
			;
	}
	puts("End 0");
	return 0;
}

DWORD WINAPI inc_1()
{
	int i;
	for(i=0; i<CPU; ++i)
	{
		A[SIZE-1] = 0;
		while(A[SIZE-1]++<N)
			;
	}
	puts("End 1");
	return 0;
}

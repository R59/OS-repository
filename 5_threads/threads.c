#include <stdio.h>
#include <time.h>
#include <windows.h>

#define CPU 2000000000
#define maxDist 50

char A[maxDist+1];
int index2;

DWORD_PTR findBit(DWORD_PTR mask, DWORD_PTR from);
DWORD WINAPI inc_0();
DWORD WINAPI inc_1();

int main()
{
	HANDLE threads[2];
	DWORD_PTR mask[2];
	void *func[] = {&inc_0, &inc_1};

	DWORD_PTR procAffMask, sysAffMask;
	if( !GetProcessAffinityMask(GetCurrentProcess(), &procAffMask, &sysAffMask) )
	{
		puts("Error: get mask");
		return 1;
	}

	mask[0] = findBit(procAffMask, 1);
	mask[1] = findBit(procAffMask, mask[0]<<1);
	if(!mask[0] || !mask[1])
	{
		puts("Not enough logical processors");
		return 2;
	}

	puts("Dist\tTime");
	for(index2=0; index2<=maxDist; ++index2)
	{
		int i;
		for(i=0; i<2; ++i)
		{
			threads[i] = CreateThread(NULL, 0, func[i], NULL, CREATE_SUSPENDED, NULL);
			if(threads[i] == NULL)
			{
				puts("Error: create");
				return 2;
			}

			if( !SetThreadAffinityMask(threads[i], mask[i]) )
			{
				puts("Error: set affinity");
				return 3;
			}
		}

		ResumeThread(threads[0]);
		ResumeThread(threads[1]);
		time_t t1 = time(NULL);
		WaitForMultipleObjects(2, threads, TRUE, INFINITE);
		time_t t2 = time(NULL);
		CloseHandle(threads[0]);
		CloseHandle(threads[1]);

		printf("%d\t%d\n", index2, (int)(t2-t1));
	}

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
		++A[0];
	return 0;
}

DWORD WINAPI inc_1()
{
	int i;
	for(i=0; i<CPU; ++i)
		++A[index2];
	return 0;
}

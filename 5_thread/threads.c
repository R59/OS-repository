#include <stdio.h>
#include <time.h>
#include <windows.h>

#define CPU 1000000000
#define N 1
#define SIZE 1	// 1000

int A[SIZE];

DWORD WINAPI inc_0();
DWORD WINAPI inc_1();

int main()
{
	HANDLE threads[2];
	void *func[] = {&inc_0, &inc_1};

	int i;
	for(i=0; i<2; ++i)
	{
		threads[i] = CreateThread(NULL, 0, func[i], NULL, CREATE_SUSPENDED, NULL);
		if(threads[i] == NULL)
		{
			puts("Error create");
			return 1;
		}
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

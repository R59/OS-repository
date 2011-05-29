#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <pthread.h>

#define SIZE 10
#define MYPORT 12345

/* ========  ========  ========  ========  ========  ========  ======== */

char startField[SIZE][SIZE+1] = {	"##########",
									"#        #",
									"#  *  *  #",
									"# ****** #",
									"#  *  *  #",
									"# ****** #",
									"#  *  *  #",
									"#        #",
									"#        #",
									"##########"	};

int curField = 0;
int count = 0;
char ***fields = NULL;
int *lock = NULL;

/* ========  ========  ========  ========  ========  ========  ======== */

char **add_field();
char **create_field();
void modify_data();	// SIGALRM
void change_life(char **cur, char **new);
void *send_data(void *x);	// thread
void printlog(char *msg);


int main()	// port9nka != good
{
	char **field = add_field();
	if(field == NULL)
	{
		puts("Not enough memory for first block");
		return 1;
	}
	for(int i=0; i<SIZE; ++i)
		memcpy(field[i], startField[i], SIZE);
	++lock[curField];


	// Set handler for SIG_ALARM
	struct sigaction S;
	S.sa_handler = modify_data;
	S.sa_flags = 0;
	if(sigemptyset(&S.sa_mask)==-1)
	{
		puts("Can't set sa_mask");
		return 2;
	}
	if(sigaction(SIGALRM, &S, NULL)==-1)
	{
		puts("Can't set signal");
		return 3;
	}


	// Set periodic send of SIG_ALARM
	struct itimerval T;
	T.it_interval.tv_sec = 1;
	T.it_interval.tv_usec = 0;
	T.it_value.tv_sec = 1;
	T.it_value.tv_usec = 0;
	if(setitimer(ITIMER_REAL, &T, NULL))
	{
		puts("Can't set timer");
		return 4;
	}


	// Create server socket
	int server = socket(PF_INET, SOCK_STREAM, 0);
	if(server == -1)
	{
		puts("Error: socket");
		return 10;
	}

	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(MYPORT);
	s_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(server, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
	{
		puts("Error: bind");
		return 11;
	}

	if(listen(server, SOMAXCONN) == -1)
	{
		puts("Error: listen");
		return 12;
	}


	// Wait client
	int client;
	do
	{
		client = accept(server, NULL, NULL);
		if(client>=0)
		{
			printlog("client connected");
			int *attr = (int *)malloc(2*sizeof(int));
			if(attr == NULL)
			{
				printlog("Error: not enough memory for arguments = close client");
				close(client);
				continue;
			}
			attr[0] = client;
			attr[1] = curField;
			pthread_t x;
			if(pthread_create(&x, NULL, send_data, (void *)attr))
			{
				printlog("Error: pthread");
				close(client);
			}
		}
		else if(client==-1 && errno!=EINTR)
		{
			printlog("Error: accept");
			client = 0;
		}
		else
			client = 0;
	}
	while(client>=0);


	// Close server socket
	printlog("closing server socket");
	if(shutdown(server, SHUT_RDWR) == -1)
		printlog("Error: shutdown");
	if(close(server) == -1)
		printlog("Error: close");


	// Free all data
	free(lock);
	for(int i=0; i<count; ++i)
	{
		for(int j=0; j<SIZE; ++j)
			free(fields[i][j]);
		free(fields[i]);
	}
	free(fields);
	return 0;
}

/* ========  ========  ========  ========  ========  ========  ======== */
/* ========  ========  ========  ========  ========  ========  ======== */

char **add_field()
{
	void *tmp = realloc(lock, (count+1)*sizeof(int));
	if(tmp == NULL)
		return NULL;
	lock = (int *)tmp;

	tmp = realloc(fields, (count+1)*sizeof(char **));
	if(tmp == NULL)
		return NULL;
	fields = (char ***)tmp;

	lock[count] = 0;
	fields[count] = create_field();
	if(fields[count] == NULL)
		return NULL;

	return fields[count++];
}
/* ========  ========  ========  ========  ========  ========  ======== */
char **create_field()
{
	char **field = (char **)malloc(SIZE*sizeof(char *));
	if(field == NULL)
		return NULL;

	for(int i=0; i<SIZE; ++i)
	{
		field[i] = (char *)malloc(SIZE*sizeof(char));
		if(field[i] == NULL)
		{
			while(i)
				free(field[--i]);
			free(field);
			return NULL;
		}
	}

	return field;
}

/* ========  ========  ========  ========  ========  ========  ======== */
/* ========  ========  ========  ========  ========  ========  ======== */

void modify_data()	// SIGALRM
{
	// find free block
	int next = 0;
	while(next<count && lock[next])
		++next;

	if(next==count && add_field()==NULL)
	{
		printlog("Not enought memory for next step = life stopped for 1 second");
		return;
	}

	//printf("[next=%d curField=%d count=%d]\n", next, curField, count);
	//for(int i=0; i<SIZE; ++i)
	//	printlog(fields[curField][i]);

	change_life(fields[curField], fields[next]);

	--lock[curField];
	++lock[curField=next];
	printlog("life");
}
/* ========  ========  ========  ========  ========  ========  ======== */
void change_life(char **cur, char **new)
{
	memcpy(new[0],cur[0], SIZE);			// life border
	memcpy(new[SIZE-1],cur[SIZE-1], SIZE);	// life border
	for(int i=0; i<SIZE; ++i)				// life border
		new[i][0] = new[i][SIZE-1] = '#';	// life border

	for(int i=1; i<SIZE-1; ++i)
		for(int j=1; j<SIZE-1; ++j)
		{
			int x = 0;
			if(cur[i-1][j-1] == '*')	++x;
			if(cur[i-1][ j ] == '*')	++x;
			if(cur[i-1][j+1] == '*')	++x;
			if(cur[ i ][j-1] == '*')	++x;
			if(cur[ i ][j+1] == '*')	++x;
			if(cur[i+1][j-1] == '*')	++x;
			if(cur[i+1][ j ] == '*')	++x;
			if(cur[i+1][j+1] == '*')	++x;
			new[i][j] = (x>=3 && x<=4 ? '*' : ' ');
		}
}

/* ========  ========  ========  ========  ========  ========  ======== */
/* ========  ========  ========  ========  ========  ========  ======== */

void *send_data(void *x)	// thread
{
	int *attr = (int *)x;
	int client = attr[0];
	int cur = attr[1];
	free(attr);

	++lock[cur];

	printlog("sending this data:");
	for(int i=0; i<SIZE; ++i)
		printlog(fields[cur][i]);

	//for(int i=0; i<1000000000; ++i)	// slow connection
	//	;

	for(int i=0; i<SIZE; ++i)
	{
		write(client, fields[cur][i], SIZE);	// need check error
		write(client, "\r\n", 2);				// need check error
	}

	printlog("closing client socket");
	if(shutdown(client, SHUT_RDWR) == -1)
		printlog("Error: shutdown");
	if(close(client) == -1)
		printlog("Error: close");

	--lock[cur];
	return NULL;
}

/* ========  ========  ========  ========  ========  ========  ======== */
/* ========  ========  ========  ========  ========  ========  ======== */

void printlog(char *msg)
{
	write(1, msg, strlen(msg));	// need check error
	write(1, "\n", 1);			// need check error
}

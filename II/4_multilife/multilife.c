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

char **add_field();
char **create_field();
void *send_data(void *x);
void modify_data();
void change_life(char **cur, char **new);

int main()
{
	char **field = add_field();
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


	int server = socket(PF_INET, SOCK_STREAM, 0);
	if(server == -1)
	{
		puts("Error: socket");
		return 10;
	}
	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(12345);
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

	struct sockaddr_in his_addr;
	socklen_t addr_len;
	int client;
	do
	{
		client = accept(server, (struct sockaddr *)&his_addr, &addr_len);
		if(client>=0)
		{
			puts("client connected");
			pthread_t x;
			if(pthread_create(&x, NULL, send_data, (void *)client))
			{
				puts("Error: pthread");
				close(client);
			}
		}
		else if(client==-1 && errno!=EINTR)
		{
			puts("Error: accept");
		}
		else
			client = 0;
	}
	while(client>=0);

	if(shutdown(client, SHUT_RDWR)==-1)
		write(1,"Error: shutdown server\n",sizeof("Error: shutdown server"));
	if(close(server)==-1)
		puts("Error: close server");
	return 0;
}

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

char **add_field()
{
	void *tmp = realloc(lock, (count+1)*sizeof(int));
	if(tmp == NULL)
		return NULL;
	lock = (int *)tmp;
	lock[count] = 0;

	tmp = realloc(fields, (count+1)*sizeof(char **));
	if(tmp == NULL)
		return NULL;
	fields = (char ***)tmp;

	fields[count] = create_field();
	if(fields[count] == NULL)
		return NULL;

	return fields[count++];
}

void modify_data()
{
	int next;
	for(next=0; next<count; ++next)
		if(!lock[next])
			break;
	if(next==count)
		if(add_field() == NULL)
		{
			puts("Not enought memory for next step = life stopped for 1 second");
			return;
		}

	//printf("[next=%d curField=%d count=%d]\n", next, curField, count);
	//for(int i=0; i<SIZE; ++i)
	//	puts(fields[curField][i]);

	change_life(fields[curField], fields[next]);

	--lock[curField];
	curField = next;
	++lock[curField];
	puts("ok");
}

void change_life(char **cur, char **new)
{
	memcpy(new[0],cur[0], SIZE);
	memcpy(new[SIZE-1],cur[SIZE-1], SIZE);
	for(int i=0; i<SIZE; ++i)
		new[i][0] = new[i][SIZE-1] = '#';
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

void *send_data(void *x)
{
	int cur = curField;
	++lock[cur];

	//puts("sending data");
	write(1, "sending data\n", sizeof("sending data"));

	for(int i=0; i<SIZE; ++i)
		puts(fields[cur][i]);

	//for(int i=0; i<1000000000; ++i) // slow connection
	//	;

	int client = (int)x;
	for(int i=0; i<SIZE; ++i)
	{
		write(client, fields[cur][i], SIZE);
		write(client, "\r\n", 2);
	}

	//puts("closing connection");
	write(1, "closing connection\n", sizeof("closing connection"));
	if(shutdown(client, SHUT_RDWR)==-1)
		write(1,"Error: shutdown client\n",sizeof("Error: shutdown client"));
	if(close(client)==-1)
		write(1,"Error: close client\n",sizeof("Error: close client"));

	--lock[cur];
	return NULL;
}

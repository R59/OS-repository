#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define EXIT_SIG 20

int pair[2];
struct datapacket
{
        int num, signo, code, mypid, mygid, sender_pid, sender_gid, sender_uid;
};

void hello_signal(int number, siginfo_t *info, void *something);

int main()
{
        if(pipe(pair)==-1)
        {
                puts("Can't create pipe");
                return 1;
        }

        struct sigaction S;
        S.sa_sigaction = hello_signal;
        S.sa_flags = SA_SIGINFO;
        if(sigfillset(&S.sa_mask)==-1)
        {
                puts("Can't set sa_mask");
                return 2;
        }

        int i;
        for(i=1; i<32; ++i)
                if(i!=SIGKILL && i!=SIGSTOP && sigaction(i, &S, NULL))
                {
                        printf("Can't set signal %d\n", i);
                        return 3;
                }

        struct datapacket d;
        do
        {
                int readed = read(pair[0], &d, sizeof(d));
                if(readed>0)
                {
                        printf("Hello, signal %d!\n\tsigno = %d\n\tcode = %d\n", d.num, d.signo, d.code);
                        printf("\tmy pid = %d\n\tmy gid = %d\n", d.mypid, d.mygid);
                        printf("\tsender pid = %d\n\tsender gid = %d\n\tsender uid = %d\n", d.sender_pid, d.sender_gid, d.sender_uid);
                }
                else if(readed==-1 && errno!=EINTR)
                {
                        puts("read error");
                        return 4;
                }
                else
                        d.num = -1; // number!=EXIT_SIG
        }
        while(d.num != EXIT_SIG);

        puts("Exit");
        return 0;
}

void hello_signal(int number, siginfo_t *info, void *something)
{
        struct datapacket send_data;
        send_data.num           = number;
        send_data.signo         = info->si_signo;
        send_data.code          = info->si_code;
        send_data.mypid         = getpid();
        send_data.mygid         = getpgrp();
        send_data.sender_pid    = info->si_pid;
        send_data.sender_gid    = getpgid(info->si_pid);
        send_data.sender_uid    = info->si_uid;
        write(pair[1], &send_data, sizeof(send_data));
}

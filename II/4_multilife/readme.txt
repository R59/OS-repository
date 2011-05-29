close(sock) or shutdown(sock, SHUT_RDWR) ???

надо ли ставить оба?

gcc -Wall -std=gnu99 -pthread multilife.c
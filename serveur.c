/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_NAME_LEN 256
#define POOL_SIZE 5
int pids[POOL_SIZE];

void CTRLC(int sig) {
    for (int i = 0; i < POOL_SIZE; i++){
        Kill(pids[i], sig);
        Waitpid(pids[i], 0, 0);
    }
    exit(0);
}

void ftp(int connfd);

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    Signal(SIGINT, CTRLC);
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    for (int i = 0; i < POOL_SIZE; i++) {
        if ((pids[i] = Fork()) == 0) {
            Signal(SIGINT, SIG_DFL);
            while (1) {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                if (connfd == -1)
                    continue;
                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                    
                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                        INET_ADDRSTRLEN);
                    
                printf("server connected to %s (%s)\n", client_hostname,
                    client_ip_string);

                ftp(connfd);
                Close(connfd);
            }
            exit(0);
        }
    }
    while(1){
        pause();
    }
    exit(0);
}


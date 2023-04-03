/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "protocole.h"

#define NB_PROC 5
#define MAX_NAME_LEN 256

int pids[NB_PROC];
int is_fils = 0;

/*fonction appeler dans le main pour créer les fils*/
void creerfils()
{
    for (int i = 0; i < NB_PROC; i++)
    {
        if ((pids[i] = Fork()) == 0)
        {
            is_fils = 1;
        }
    }
}
/*handler pour tuer les fils*/
void SIGINT_handler(int sig)
{
    for (int i = 0; i < NB_PROC; i++)
    {
        Kill(pids[i], sig);
        Waitpid(pids[i], 0, 0);
    }
    exit(0);
}

void SIGPIPE_handler(int sig);

void ftp(int connfd);

/*
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    Signal(SIGINT, SIGINT_handler);
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(NUMPORT);
    creerfils();
    if (is_fils){
        /*fils*/
        Signal(SIGINT, SIG_DFL);
        Signal(SIGPIPE, SIGPIPE_handler);
        while (1){
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            if (connfd == -1)
                continue;
            /* determine the name of the client */
            Getnameinfo((SA *)&clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);

            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                      INET_ADDRSTRLEN);

            printf("server connected to %s (%s)\n", client_hostname,
                   client_ip_string);

            /*appel à la fonction ftp qui gère l'échange entre le serveur et client*/
            ftp(connfd);
            Close(connfd);
        }
        exit(0);
    }
    else {
        /*père*/
        while (1)
        {
            pause();
        }
        exit(0);
    }
}

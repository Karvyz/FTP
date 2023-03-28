#include "csapp.h"
#include "protocole.h"
#include "readcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_file(Cmdline *l, int clientfd) {
    char* nom_fichier = l->seq[0][1];
    if (nom_fichier == NULL) {
        return;
    }

    Requete_client rc;
    rc.type = GET;
    rc.taille = strlen(nom_fichier);

    Rio_writen(clientfd, &rc, sizeof(rc));
    Rio_writen(clientfd, nom_fichier, rc.taille);

    Get_reponse reponse;
    Rio_readn(clientfd, &reponse, sizeof(Get_reponse));
    if (reponse.erreur != AUCUNE) {
        fprintf(stderr, "erreur %d", reponse.erreur);
        exit(EXIT_FAILURE);
    }
    char* buffer = Malloc(reponse.taille_fichier);
    Rio_readn(clientfd, buffer, reponse.taille_fichier);

    int new_file = Open(nom_fichier, O_CREAT | O_WRONLY, 0644);
    Rio_writen(new_file, buffer, reponse.taille_fichier);
    free(buffer);
}

void client(int clientfd) {
    printf("client connected to server OS\n"); 
    Cmdline *l;
    while (1) {
        printf("FTP>");
        l = readcmd();
        
        // If input stream closed, normal termination
        if (!l) {
            exit(0);    // No need to free l before exit, readcmd() already did it
        }

        // Syntax error, read another command
        if (l->err) {
            fprintf(stderr, "synthax error: %s\n", l->err);
            continue;
        }

        // Empty command
        if (!l->seq[0])
            continue;
        
        if (strcmp(l->seq[0][0], "GET") == 0) {
            get_file(l, clientfd);
        }
    }
}

int main(int argc, char **argv) {
    int clientfd, port;
    char *host;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);

    client(clientfd);
    
    Close(clientfd);
    exit(0);
}

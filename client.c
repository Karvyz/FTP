#include "csapp.h"
#include "protocole.h"
#include "readcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Procedure pour recevoir un fichier
void GET_fichier(Cmdline *l, int clientfd) {
    char* nom_fichier = l->seq[0][1];
    if (nom_fichier == NULL) {
        return;
    }

    // On cree une requete "GET" qui contient la taille du nom du fichier
    Requete_client rc;
    rc.type = GET;
    rc.taille = strlen(nom_fichier);

    // On envoie la requete
    Rio_writen(clientfd, &rc, sizeof(Requete_client));

    // On envoie le nom du fichier
    Rio_writen(clientfd, nom_fichier, rc.taille);

    // On verifie la réponse du serveur pour savoir si il y a eu une erreur
    // On obtient également la taille du fichier
    Get_reponse reponse;
    Rio_readn(clientfd, &reponse, sizeof(Get_reponse));
    if (reponse.erreur != AUCUNE) {
        fprintf(stderr, "erreur %d\n", reponse.erreur);
        exit(EXIT_FAILURE);
    }

    // On reçoit le fichier
    char buffer[reponse.taille_fichier];
    Rio_readn(clientfd, buffer, reponse.taille_fichier);

    // On stocke le fichier reçut dans un fichier du même nom à la racine
    int new_file = Open(nom_fichier, O_CREAT | O_WRONLY, 0644);
    Rio_writen(new_file, buffer, reponse.taille_fichier);
    //done lorsque tout est fini
    printf("\033[0;32mDone\033[0m\n");
}

void fin_communication(int clientfd) {
    // On cree une requete "END"
    Requete_client rc;
    rc.type = END;
    rc.taille = 0;

    // On envoie la requete
    Rio_writen(clientfd, &rc, sizeof(Requete_client));
}

void client(int clientfd) {
    printf("client connected to server OS\n"); 
    Cmdline *l;

    srand(time(NULL));
    // Tant que la commande n'est pas valide on recommence
    while (1) {
        // génère un nombre aléatoire pour afficher FTP> avec une couleur aléatoire 
        int random_color = rand() % 7 + 31; 
        printf("\033[0;%dmFTP>\033[0m", random_color);
        l = readcmd();
        
        // On ferme si stdin est fermé
        if (!l) {
            exit(0);
        }

        // Erreur de syntaxe dans la commande
        if (l->err) {
            fprintf(stderr, "synthax error: %s\n", l->err);
            continue;
        }

        // Commande vide
        if (!l->seq[0])
            continue;
        
        // Si on cherche à obtenir un fichier
        if (strcmp(l->seq[0][0], "GET") == 0) {
            GET_fichier(l, clientfd);
            continue;
        }

        if (strcmp(l->seq[0][0], "BYE") == 0) {
            fin_communication(clientfd);
            break;
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

    // Connexion au serveur
    clientfd = Open_clientfd(host, port);

    // application du client
    client(clientfd);
    
    Close(clientfd);
    exit(EXIT_SUCCESS);
}

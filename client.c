#include "csapp.h"
#include "protocole.h"
#include "readcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

//petite fonction pour calculer les stats du transfert
float calculer_rapidite(double taille, double temps) {
    float kbits = taille / 1000; // conversion de la taille en Kbits
    float kbits_par_sec = kbits / temps; // calcul du débit en Kbits/s
    return kbits_par_sec;
}


// Procedure pour recevoir un fichier
void GET_fichier(Cmdline *l, int clientfd) {
    
    struct timeval temps_debut, temps_fin;
    // temps de début
    gettimeofday(&temps_debut, NULL);

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
    //si il y a une erreur on affiche l'erreur
    if (reponse.erreur != AUCUNE) {
        switch (reponse.erreur){
            case FICHIER_NON_TROUVE:
                printf("\033[0;31mfichier inexistant\033[0m\n");
                break;

            case FICHIER_NON_ACCESSIBLE: 
                printf("\033[0;31mfichier non accessible\033[0m\n");
                break;

            default:
                break;
        }
        return;
    }

    // créer le fichier sur lequel on va écrire le fichier qu'on va recevoir
    int new_file;
    if (access(nom_fichier, F_OK) == 0) {
        new_file = Open(nom_fichier, O_APPEND | O_WRONLY, 0644);
    }
    else {
        new_file = Open(nom_fichier, O_CREAT | O_WRONLY, 0644);
    }

    struct stat infos_fichier;
    Fstat(new_file, &infos_fichier);
    long taille_fichier = infos_fichier.st_size;
    rio_writen(clientfd, &taille_fichier, sizeof(long));

    long taille_restant = reponse.taille_fichier - taille_fichier;

    // calcul du nombre de packet que l'on va recevoir
    long nb_packet = (taille_restant / TAILLE_BUFFER) + ((taille_restant % TAILLE_BUFFER) > 0 ? 1 : 0);

    /* boucle pour la reception des packets
        nb_packet sera le nombre de paquet qu'il reste à recevoir*/
    while (nb_packet>0)
    {   
        /*taille_effective sera la taille du bloc qu'on recoit:
            elle est égale à taille buffer (qu'on a recu plus tot) pour tous les blocs
            sauf le dernier qui prendra le nombre d'octets restant*/
        long taille_effective;
        if (nb_packet==1){
            //calcul du nombre d'octet du dernier bloc
            taille_effective= taille_restant%TAILLE_BUFFER;
        }
        else {taille_effective=TAILLE_BUFFER;}

        //creation buffer de taille=taille_effective
        char buffer[TAILLE_BUFFER];
        //on recoit le bloc qu'on stock dans le buffer
        Rio_readn(clientfd, buffer, taille_effective);
        //on écrit le bloc dans le fichier
        Rio_writen(new_file, buffer, taille_effective);
        nb_packet--;
    }

    // temps de fin
    gettimeofday(&temps_fin, NULL); // temps de fin
    
    double temps_ecoule = (temps_fin.tv_sec - temps_debut.tv_sec) + (double)(temps_fin.tv_usec - temps_debut.tv_usec) / 1000000.0; // temps écoulé en secondes
    double rapidite = calculer_rapidite(reponse.taille_fichier, temps_ecoule);

    //print done lorsque tout est fini
    printf("\033[0;32mTransfer successfully complete :\033[0m\n");
    printf("%ld bytes ont été reçu en %.3lf secondes (%.2lf Kbytes/s)\n",reponse.taille_fichier, temps_ecoule, rapidite);
    
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
        if (strcmp(l->seq[0][0], "get") == 0) {

            GET_fichier(l, clientfd);
            continue;
        }

        if (strcmp(l->seq[0][0], "bye") == 0) {
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
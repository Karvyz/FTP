#include "csapp.h"
#include "protocole.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int test_fichier(char* nom_fichier){
    if (access(nom_fichier, F_OK) != 0) {
        return 1;
    }
    if (access(nom_fichier, R_OK) != 0) {
        return 2;
    }
    return 0;
}

void get_f(int connfd, Requete_client requete){
    char* nom_fichier = malloc(requete.taille + 1);
    Rio_readn(connfd, nom_fichier, requete.taille);
    nom_fichier[requete.taille] = '\0';
    char path[] = "files/";
    strcat(path, nom_fichier);


    Get_reponse reponse;
    if ((reponse.erreur = test_fichier(path)) != 0){
        reponse.taille_fichier = 0;
        Rio_writen(connfd, &reponse, sizeof(reponse));
    }
    else {
        int fichier = open(path, O_RDONLY, 0);
        struct stat infos_fichier;
        Fstat(fichier, &infos_fichier);
        int32_t taille = infos_fichier.st_size;
        reponse.taille_fichier = taille;
        Rio_writen(connfd, &reponse, sizeof(reponse));
        char buffer[taille];
        Rio_readn(fichier, buffer, taille);
        Rio_writen(connfd, buffer ,taille);
    }

}

void ftp(int connfd)
{
    /*reçoit la requette*/
    Requete_client requete;
    Rio_readn(connfd, &requete, sizeof(Requete_client));
    /*recoit le nom du fichier*/

    if (requete.type==GET){get_f(connfd, requete);}
}
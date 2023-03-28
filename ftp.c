#include "csapp.h"
#include "protocole.h"
#include <unistd.h>
#include <sys/stat.h>

int test_fichier(char* nom_fichier){
    if (access(nom_fichier, F_OK) == 0) {
        return 1;
    }
    if (access(nom_fichier, R_OK) == 0) {
        return 2;
    }
    return 0;
}

void get_f(int connfd, char* nom_fichier){
    Get_reponse reponse;
    if ((reponse.erreur = test_fichier(nom_fichier)) != 0){
        reponse.taille_fichier = 0;
        Rio_writen(connfd, &reponse, sizeof(reponse));
    }
    else {
        int fichier = Open(nom_fichier, O_RDONLY, 0);
        struct stat infos_fichier;
        Fstat(fichier, &infos_fichier);
        int32_t taille = infos_fichier.st_size;
        char buffer[taille];
        Rio_readn(fichier, buffer, taille);
        Rio_writen(connfd, buffer ,taille);
    }

}

void ftp(int connfd)
{
    /*reçoit la requette*/
    Requete_client requete;
    Rio_readn(connfd, &requete, sizeof(requete));
    /*recoit le nom du fichier*/
    char* nom_fichier = malloc(requete.taille);
    Rio_readn(connfd, &nom_fichier, requete.taille);

    if (requete.type==0){get_f(connfd, nom_fichier);}
}
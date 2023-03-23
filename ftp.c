#include "csapp.h"
#include "protocole.h"
#include <unistd.h>

int test_fichier(Requete requete){
    if (access(requete.nom_fichier, F_OK) == 0) {
        return 1;
    }
    if (access(requete.nom_fichier, R_OK) == 0) {
        return 2;
    }
    return 0;
}

void ftp(int connfd)
{
    Requete requete;
    Rio_readn(connfd, &requete, sizeof(requete));
    Get_reponse reponse;
    if ((reponse.erreur = test_fichier(requete)) != 0){
        reponse.taille_fichier = 0;
        Rio_writen(connfd, &reponse, sizeof(reponse));
    }
    else {
        int fichier = Open(requete.nom_fichier, O_RDONLY, 0);
        char buffer[8192];
        Fstat(fichier, struct stat *buf)
        Rio_readn(int fd, void *usrbuf, size_t n)
    }
}
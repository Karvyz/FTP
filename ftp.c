#include "csapp.h"
#include "protocole.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define TAILLE_BUFFER 8192

/*test si le fichier est accessibles :
    renvoi 1 si inexistant
    renvoi 2 si inaccessible en lecture
    renvoi 0 si accessible*/
int test_fichier(char* nom_fichier){
    if (access(nom_fichier, F_OK) != 0) {
        return 1;
    }
    if (access(nom_fichier, R_OK) != 0) {
        return 2;
    }
    return 0;
}


/* fonction appeler par ftp
reçoit la requette GET,
envoie le fichier voulu en envoyant d'abord sa taille puis le fichier
renvoi une erreur si fichier inexistant ou innaccessible*/
void get_f(int connfd, Requete_client requete){


    /*creation du buffer de taille qu'on a reçu dans requete */
    char* nom_fichier = malloc(requete.taille + 1);

    /*stock le nom du fichier dans le buffer*/
    Rio_readn(connfd, nom_fichier, requete.taille);

    /* rajout de \0 pour définir la fin du nom*/
    nom_fichier[requete.taille] = '\0';
    /*on va chercher les fichiers dans le dossier files*/
    char path[] = "files/";
    strcat(path, nom_fichier);


    Get_reponse reponse;

    if ((reponse.erreur = test_fichier(path)) != 0){
        /*si le fichier est innaccessible renvoie la reponse avec taille=0 et le numero d'erreur*/
        reponse.taille_fichier = 0;
        Rio_writen(connfd, &reponse, sizeof(reponse));
    }
    else {
        /*le fichier est accessible*/
        int fichier = open(path, O_RDONLY, 0);
        /*création d'une structure stat pour stocker la taille du fichier*/
        struct stat infos_fichier;
        Fstat(fichier, &infos_fichier);
        int taille_fichier = infos_fichier.st_size;
        reponse.taille_fichier = taille_fichier;

        /*renvoi la reponse qui contient : la taille du fichier et erreur =0*/
        Rio_writen(connfd, &reponse, sizeof(reponse));
        char buffer[TAILLE_BUFFER];
        // on envoie la taille du buffer
        int taille_buffer= TAILLE_BUFFER;
        Rio_writen(connfd, &taille_buffer, sizeof(int));
        // on calcule le nombre de packet qu'on va envoyer
        int nb_packet = (taille_fichier/TAILLE_BUFFER) + ((taille_fichier%TAILLE_BUFFER) > 0 ? 1 : 0);
        /* boucle pour l'envoi des packets
         nb_packet sera le nombre de paquet qu'il reste à envoyer*/
        while (nb_packet>0)
            {   
            /*taille_effective sera la taille du bloc qu'on envoi:
            elle est égale à taille buffer pour tous sauf le dernier qui prendra le nombre d'octets restant*/
            int taille_effective;
            if (nb_packet==1)
                {taille_effective = taille_fichier%TAILLE_BUFFER;}
            else {taille_effective= TAILLE_BUFFER;}
            // on lis le fichier qu'on stock dans le buffer
            Rio_readn(fichier, buffer, taille_effective);
            //on écrit dans la socket le buffer
            Rio_writen(connfd, buffer ,taille_effective);
            nb_packet--;
        }
    }
}

void ftp(int connfd) {
    while (1) {
        /*reçoit la requette*/
        Requete_client requete;
        Rio_readn(connfd, &requete, sizeof(Requete_client));

        switch (requete.type) {
            case GET: // copier un fichier du serveur
                get_f(connfd, requete);
                break;
            case PUT:// copier un fichier vers le serveur
                break;
            case END: // Fin de la communication
                printf("End communication\n");
                return;
        }
    }
}
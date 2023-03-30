#include <stdint.h>
#define NUMPORT 2121
#define TAILLE_BUFFER 8192



enum codes_erreurs {
    AUCUNE = 0,
    FICHIER_NON_TROUVE = 1,
    FICHIER_NON_ACCESSIBLE = 2
};

enum type_requette {
    GET = 0,
    END = 1,
};

// 1) structure de la première requete que le client va envoyer
typedef struct {
    enum type_requette type;
    int32_t taille;
} Requete_client;

// 2) Envoi d'un char de taille définie dans la requete précedente

// 3) structure de la réponse du serveur au client
typedef struct {
    enum codes_erreurs erreur;
    int32_t taille_fichier;
} Get_reponse;

// 4) On envoie tous les paquets de données
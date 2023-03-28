#include <stdint.h>
#define NUMPORT 2121

enum codes_erreurs {
    AUCUNE = 0,
    FICHIER_NON_TROUVE = 1,
    FICHIER_NON_ACCESSIBLE = 2,
    ERREUR_INCONNUE = 3
};

enum type_requette {
    GET = 0,
    END = 1,
    PUT = 2
};

/*structure de la première requete que le client va envoyer*/
typedef struct {
    enum type_requette type;
    int32_t taille;
} Requete_client;

/*structure de la réponse du serveur au client*/
typedef struct {
    enum codes_erreurs erreur;
    int32_t taille_fichier;
} Get_reponse;

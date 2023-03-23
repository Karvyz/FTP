#include <stdint.h>

enum codes_erreurs {
    AUCUNE = 0,
    FICHIER_NON_TROUVE = 1,
    FICHIER_NON_ACCESSIBLE = 2,
    ERREUR_INCONNUE = 3
};

typedef struct {
    char nom_fichier[256];
} Requete;

typedef struct {
    enum codes_erreurs erreur;
    int32_t taille_fichier;
} Get_reponse;

typedef struct {
    char buffer[8192];
} Transfert;
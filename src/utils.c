#include "utils.h"

void afficher_menu() {
    printf("\n");
    printf("========================================\n");
    printf("     SYSTEME DE GESTION NETFLOW        \n");
    printf("========================================\n");
    printf("1. Charger un reseau depuis un fichier\n");
    printf("2. Afficher le graphe du reseau\n");
    printf("3. Calculer le plus court chemin (Dijkstra)\n");
    printf("4. Trouver tous les chemins (Backtracking)\n");
    printf("5. Optimiser le flux reseau\n");
    printf("6. Analyser la securite du reseau\n");
    printf("7. Verifier la connectivite\n");
    printf("0. Quitter\n");
    printf("========================================\n");
    printf("Votre choix: ");
}

void nettoyer_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int lire_entier(const char* message) {
    int valeur;
    printf("%s", message);
    while (scanf("%d", &valeur) != 1) {
        nettoyer_buffer();
        printf("Erreur: entrez un nombre entier valide.\n");
        printf("%s", message);
    }
    nettoyer_buffer();
    return valeur;
}

char* lire_chaine(const char* message, int max_length) {
    char* chaine = (char*)malloc(max_length * sizeof(char));
    if (!chaine) return NULL;
    
    printf("%s", message);
    if (fgets(chaine, max_length, stdin) != NULL) {
        size_t len = strlen(chaine);
        if (len > 0 && chaine[len - 1] == '\n') {
            chaine[len - 1] = '\0';
        }
    }
    
    return chaine;
}

void afficher_erreur(const char* message) {
    printf("\n[ERREUR] %s\n", message);
}

void afficher_succes(const char* message) {
    printf("\n[SUCCES] %s\n", message);
}

void afficher_info(const char* message) {
    printf("\n[INFO] %s\n", message);
}

double get_temps_execution(clock_t debut, clock_t fin) {
    return ((double)(fin - debut)) / CLOCKS_PER_SEC;
}

#include "graphe.h"
#include <string.h>

Graphe* creer_graphe(int nb_sommets) {
    Graphe* g = (Graphe*)malloc(sizeof(Graphe));
    if (!g) return NULL;
    
    g->nb_sommets = nb_sommets;
    g->sommets = (Sommet*)malloc(nb_sommets * sizeof(Sommet));
    
    if (!g->sommets) {
        free(g);
        return NULL;
    }
    
    for (int i = 0; i < nb_sommets; i++) {
        g->sommets[i].id = i;
        sprintf(g->sommets[i].nom, "Noeud_%d", i);
        g->sommets[i].aretes = NULL;
    }
    
    return g;
}

void ajouter_arete(Graphe* g, int source, int dest, int latence, int bande_passante, int cout, int securite) {
    if (!g || source < 0 || source >= g->nb_sommets || dest < 0 || dest >= g->nb_sommets) {
        return;
    }
    
    Arete* nouvelle_arete = (Arete*)malloc(sizeof(Arete));
    if (!nouvelle_arete) return;
    
    nouvelle_arete->destination = dest;
    nouvelle_arete->latence = latence;
    nouvelle_arete->bande_passante = bande_passante;
    nouvelle_arete->cout = cout;
    nouvelle_arete->securite = securite;
    nouvelle_arete->debit = 0;
    nouvelle_arete->suivant = g->sommets[source].aretes;
    g->sommets[source].aretes = nouvelle_arete;
}

void afficher_graphe(Graphe* g) {
    if (!g) return;
    
    printf("\n=== Graphe du reseau ===\n");
    printf("Nombre de sommets: %d\n\n", g->nb_sommets);
    
    for (int i = 0; i < g->nb_sommets; i++) {
        printf("Sommet %d (%s):\n", g->sommets[i].id, g->sommets[i].nom);
        Arete* arete = g->sommets[i].aretes;
        
        if (!arete) {
            printf("  Aucune connexion\n");
        } else {
            while (arete) {
                printf("  -> %d (latence: %dms, BP: %dMbps, cout: %d, securite: %d/10)\n",
                       arete->destination, arete->latence, arete->bande_passante, 
                       arete->cout, arete->securite);
                arete = arete->suivant;
            }
        }
        printf("\n");
    }
}

void liberer_graphe(Graphe* g) {
    if (!g) return;
    
    for (int i = 0; i < g->nb_sommets; i++) {
        Arete* arete = g->sommets[i].aretes;
        while (arete) {
            Arete* temp = arete;
            arete = arete->suivant;
            free(temp);
        }
    }
    
    free(g->sommets);
    free(g);
}

Graphe* charger_graphe_fichier(const char* nom_fichier) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }
    
    int nb_sommets, nb_aretes;
    if (fscanf(fichier, "%d %d", &nb_sommets, &nb_aretes) != 2) {
        fclose(fichier);
        return NULL;
    }
    
    Graphe* g = creer_graphe(nb_sommets);
    if (!g) {
        fclose(fichier);
        return NULL;
    }
    
    for (int i = 0; i < nb_aretes; i++) {
        int source, dest, latence, bande_passante, cout, securite;
        if (fscanf(fichier, "%d %d %d %d %d %d", &source, &dest, &latence, &bande_passante, &cout, &securite) == 6) {
            ajouter_arete(g, source, dest, latence, bande_passante, cout, securite);
        }
    }
    
    fclose(fichier);
    return g;
}

void supprimer_arete(Graphe* g, int source, int dest) {
    if (!g || source < 0 || source >= g->nb_sommets) return;
    
    Arete* courant = g->sommets[source].aretes;
    Arete* precedent = NULL;
    
    while (courant) {
        if (courant->destination == dest) {
            if (precedent) {
                precedent->suivant = courant->suivant;
            } else {
                g->sommets[source].aretes = courant->suivant;
            }
            free(courant);
            return;
        }
        precedent = courant;
        courant = courant->suivant;
    }
}

void supprimer_sommet(Graphe* g, int sommet) {
    if (!g || sommet < 0 || sommet >= g->nb_sommets) return;
    
    for (int i = 0; i < g->nb_sommets; i++) {
        if (i != sommet) {
            supprimer_arete(g, i, sommet);
        }
    }
    
    Arete* arete = g->sommets[sommet].aretes;
    while (arete) {
        Arete* temp = arete;
        arete = arete->suivant;
        free(temp);
    }
    g->sommets[sommet].aretes = NULL;
}

void sauvegarder_graphe_fichier(Graphe* g, const char* nom_fichier) {
    if (!g || !nom_fichier) return;
    
    FILE* fichier = fopen(nom_fichier, "w");
    if (!fichier) {
        printf("Erreur: impossible de creer le fichier %s\n", nom_fichier);
        return;
    }
    
    int nb_aretes = 0;
    for (int i = 0; i < g->nb_sommets; i++) {
        Arete* arete = g->sommets[i].aretes;
        while (arete) {
            nb_aretes++;
            arete = arete->suivant;
        }
    }
    
    fprintf(fichier, "%d %d\n", g->nb_sommets, nb_aretes);
    
    for (int i = 0; i < g->nb_sommets; i++) {
        Arete* arete = g->sommets[i].aretes;
        while (arete) {
            fprintf(fichier, "%d %d %d %d %d %d\n", 
                    i, arete->destination, arete->latence, 
                    arete->bande_passante, arete->cout, arete->securite);
            arete = arete->suivant;
        }
    }
    
    fclose(fichier);
    printf("Graphe sauvegarde dans %s\n", nom_fichier);
}

void afficher_matrice_adjacence(Graphe* g) {
    if (!g) return;
    
    printf("\n=== Matrice d'Adjacence (Latence) ===\n");
    printf("     ");
    for (int i = 0; i < g->nb_sommets; i++) {
        printf("%4d ", i);
    }
    printf("\n");
    
    for (int i = 0; i < g->nb_sommets; i++) {
        printf("%4d ", i);
        for (int j = 0; j < g->nb_sommets; j++) {
            int trouve = 0;
            Arete* arete = g->sommets[i].aretes;
            while (arete) {
                if (arete->destination == j) {
                    printf("%4d ", arete->latence);
                    trouve = 1;
                    break;
                }
                arete = arete->suivant;
            }
            if (!trouve) {
                printf("   - ");
            }
        }
        printf("\n");
    }
}

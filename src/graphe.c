#include "graphe.h"
#include <string.h>

Graphe* creer_graphe(int nb_noeuds) {
    Graphe* g = (Graphe*)malloc(sizeof(Graphe));
    if (!g) return NULL;
    
    g->nb_noeuds = nb_noeuds;
    g->noeuds = (Noeud*)malloc(nb_noeuds * sizeof(Noeud));
    
    if (!g->noeuds) {
        free(g);
        return NULL;
    }
    
    g->matrice_adjacence = (float**)malloc(nb_noeuds * sizeof(float*));
    if (!g->matrice_adjacence) {
        free(g->noeuds);
        free(g);
        return NULL;
    }
    
    for (int i = 0; i < nb_noeuds; i++) {
        g->matrice_adjacence[i] = (float*)calloc(nb_noeuds, sizeof(float));
        if (!g->matrice_adjacence[i]) {
            for (int j = 0; j < i; j++) free(g->matrice_adjacence[j]);
            free(g->matrice_adjacence);
            free(g->noeuds);
            free(g);
            return NULL;
        }
    }
    
    for (int i = 0; i < nb_noeuds; i++) {
        g->noeuds[i].id = i;
        sprintf(g->noeuds[i].nom, "Noeud_%d", i);
        g->noeuds[i].aretes = NULL;
    }
    
    return g;
}

void ajouter_arete(Graphe* g, int source, int dest, float latence, float bande_passante, float cout, int securite) {
    if (!g || source < 0 || source >= g->nb_noeuds || dest < 0 || dest >= g->nb_noeuds) {
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
    nouvelle_arete->suivant = g->noeuds[source].aretes;
    g->noeuds[source].aretes = nouvelle_arete;
    
    g->matrice_adjacence[source][dest] = latence;
}

void afficher_graphe(Graphe* g) {
    if (!g) return;
    
    printf("\n=== Graphe du reseau ===\n");
    printf("Nombre de noeuds: %d\n\n", g->nb_noeuds);
    
    for (int i = 0; i < g->nb_noeuds; i++) {
        printf("Noeud %d (%s):\n", g->noeuds[i].id, g->noeuds[i].nom);
        Arete* arete = g->noeuds[i].aretes;
        
        if (!arete) {
            printf("  Aucune connexion\n");
        } else {
            while (arete) {
                printf("  -> %d (latence: %.1fms, BP: %.0fMbps, cout: %.1f, securite: %d/10)\n",
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
    
    for (int i = 0; i < g->nb_noeuds; i++) {
        Arete* arete = g->noeuds[i].aretes;
        while (arete) {
            Arete* temp = arete;
            arete = arete->suivant;
            free(temp);
        }
    }
    
    if (g->matrice_adjacence) {
        for (int i = 0; i < g->nb_noeuds; i++) {
            free(g->matrice_adjacence[i]);
        }
        free(g->matrice_adjacence);
    }
    
    free(g->noeuds);
    free(g);
}

Graphe* charger_graphe_fichier(const char* nom_fichier) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }
    
    int nb_noeuds, nb_aretes;
    if (fscanf(fichier, "%d %d", &nb_noeuds, &nb_aretes) != 2) {
        fclose(fichier);
        return NULL;
    }
    
    Graphe* g = creer_graphe(nb_noeuds);
    if (!g) {
        fclose(fichier);
        return NULL;
    }
    
    for (int i = 0; i < nb_aretes; i++) {
        int source, dest, securite;
        float latence, bande_passante, cout;
        if (fscanf(fichier, "%d %d %f %f %f %d", &source, &dest, &latence, &bande_passante, &cout, &securite) == 6) {
            ajouter_arete(g, source, dest, latence, bande_passante, cout, securite);
        }
    }
    
    fclose(fichier);
    return g;
}

void supprimer_arete(Graphe* g, int source, int dest) {
    if (!g || source < 0 || source >= g->nb_noeuds) return;
    
    Arete* courant = g->noeuds[source].aretes;
    Arete* precedent = NULL;
    
    while (courant) {
        if (courant->destination == dest) {
            if (precedent) {
                precedent->suivant = courant->suivant;
            } else {
                g->noeuds[source].aretes = courant->suivant;
            }
            g->matrice_adjacence[source][dest] = 0;
            free(courant);
            return;
        }
        precedent = courant;
        courant = courant->suivant;
    }
}

void supprimer_sommet(Graphe* g, int sommet) {
    if (!g || sommet < 0 || sommet >= g->nb_noeuds) return;
    
    for (int i = 0; i < g->nb_noeuds; i++) {
        if (i != sommet) {
            supprimer_arete(g, i, sommet);
        }
    }
    
    Arete* arete = g->noeuds[sommet].aretes;
    while (arete) {
        Arete* temp = arete;
        arete = arete->suivant;
        free(temp);
    }
    g->noeuds[sommet].aretes = NULL;
}

void sauvegarder_graphe_fichier(Graphe* g, const char* nom_fichier) {
    if (!g || !nom_fichier) return;
    
    FILE* fichier = fopen(nom_fichier, "w");
    if (!fichier) {
        printf("Erreur: impossible de creer le fichier %s\n", nom_fichier);
        return;
    }
    
    int nb_aretes = 0;
    for (int i = 0; i < g->nb_noeuds; i++) {
        Arete* arete = g->noeuds[i].aretes;
        while (arete) {
            nb_aretes++;
            arete = arete->suivant;
        }
    }
    
    fprintf(fichier, "%d %d\n", g->nb_noeuds, nb_aretes);
    
    for (int i = 0; i < g->nb_noeuds; i++) {
        Arete* arete = g->noeuds[i].aretes;
        while (arete) {
            fprintf(fichier, "%d %d %.1f %.0f %.1f %d\n", 
                    i, arete->destination, arete->latence, 
                    arete->bande_passante, arete->cout, arete->securite);
            arete = arete->suivant;
        }
    }
    
    fclose(fichier);
    printf("Graphe sauvegarde dans %s\n", nom_fichier);
}

void afficher_matrice_adjacence(Graphe* g) {
    if (!g || !g->matrice_adjacence) return;
    
    printf("\n=== Matrice d'Adjacence (Latence) ===\n");
    printf("      ");
    for (int i = 0; i < g->nb_noeuds; i++) {
        printf("%6d ", i);
    }
    printf("\n");
    
    for (int i = 0; i < g->nb_noeuds; i++) {
        printf("%4d  ", i);
        for (int j = 0; j < g->nb_noeuds; j++) {
            if (g->matrice_adjacence[i][j] > 0) {
                printf("%6.1f ", g->matrice_adjacence[i][j]);
            } else {
                printf("     - ");
            }
        }
        printf("\n");
    }
}

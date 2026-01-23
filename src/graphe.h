#ifndef GRAPHE_H
#define GRAPHE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Arete {
    int destination;
    float latence;
    float bande_passante;
    float cout;
    int securite;
    float debit;
    struct Arete* suivant;
} Arete;

typedef struct Noeud {
    int id;
    char nom[50];
    Arete* aretes;
} Noeud;

typedef struct Graphe {
    int nb_noeuds;
    Noeud* noeuds;
    float** matrice_adjacence;
} Graphe;

Graphe* creer_graphe(int nb_noeuds);
void ajouter_arete(Graphe* g, int source, int dest, float latence, float bande_passante, float cout, int securite);
void supprimer_arete(Graphe* g, int source, int dest);
void supprimer_sommet(Graphe* g, int sommet);
void afficher_graphe(Graphe* g);
void afficher_matrice_adjacence(Graphe* g);
void liberer_graphe(Graphe* g);
Graphe* charger_graphe_fichier(const char* nom_fichier);
void sauvegarder_graphe_fichier(Graphe* g, const char* nom_fichier);

#endif

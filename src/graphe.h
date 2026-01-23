#ifndef GRAPHE_H
#define GRAPHE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Arete {
    int destination;
    int latence;
    int bande_passante;
    int cout;
    int securite;
    int debit;
    struct Arete* suivant;
} Arete;

typedef struct Sommet {
    int id;
    char nom[50];
    Arete* aretes;
} Sommet;

typedef struct Graphe {
    int nb_sommets;
    Sommet* sommets;
} Graphe;

Graphe* creer_graphe(int nb_sommets);
void ajouter_arete(Graphe* g, int source, int dest, int latence, int bande_passante, int cout, int securite);
void supprimer_arete(Graphe* g, int source, int dest);
void supprimer_sommet(Graphe* g, int sommet);
void afficher_graphe(Graphe* g);
void afficher_matrice_adjacence(Graphe* g);
void liberer_graphe(Graphe* g);
Graphe* charger_graphe_fichier(const char* nom_fichier);
void sauvegarder_graphe_fichier(Graphe* g, const char* nom_fichier);

#endif

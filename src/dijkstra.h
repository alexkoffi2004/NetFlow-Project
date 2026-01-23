#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graphe.h"

typedef struct CheminResult {
    int* predecesseurs;
    int* distances;
    int cout_total;
    int a_cycle_negatif;
} CheminResult;

typedef struct CheminK {
    int* sommets;
    int longueur;
    int cout_total;
} CheminK;

typedef struct KCheminsResult {
    CheminK* chemins;
    int nb_chemins;
    int k;
} KCheminsResult;

CheminResult* dijkstra(Graphe* g, int source, int destination);
CheminResult* bellman_ford(Graphe* g, int source, int destination);
KCheminsResult* k_plus_courts_chemins(Graphe* g, int source, int destination, int k);
void afficher_chemin(CheminResult* resultat, int source, int destination);
void afficher_k_chemins(KCheminsResult* resultat);
void liberer_chemin(CheminResult* resultat);
void liberer_k_chemins(KCheminsResult* resultat);
int* reconstruire_chemin(CheminResult* resultat, int source, int destination, int* longueur);
double analyser_redondance(KCheminsResult* resultat);

#endif

/**
 * @file graphe.h
 * @brief Gestion de la topologie réseau et des algorithmes de routage (NetFlow Optimizer).
 */

#ifndef GRAPHE_H
#define GRAPHE_H

#include <stdio.h>
#include <stdlib.h>
#include <float.h> // Indispensable pour FLT_MAX dans les algos de routage

/* --- STRUCTURES DE DONNÉES --- */

/**
 * @struct Arete
 * @brief Représente une liaison entre deux nœuds.
 */
typedef struct Arete {
    int destination;
    float latence;
    float bande_passante;
    float cout;
    int securite;
    struct Arete* suivant;
} Arete;

typedef struct {
    int id;
    Arete* liste_adj;
} Noeud;

typedef struct {
    int nb_noeuds;
    Noeud* tab_noeuds;
} Graphe;

typedef struct {
    int* chemin;
    int taille;
    float latence_totale;
} ResultatRoutage;


/* --- PROTOTYPES DES FONCTIONS --- */

// Module 1 : Gestion du Graphe
Graphe* initialiser_graphe(int nb_noeuds);
void ajouter_arete(Graphe* g, int src, int dest, float lat, float bp, float cout, int sec);
Graphe* charger_depuis_fichier(char* nom_fichier);
void afficher_graphe(Graphe* g);
void liberer_graphe(Graphe* g);

// Module 2 : Algorithmes de Routage
/** @brief Calcule le chemin le plus rapide (latence min) */
void dijkstra(Graphe* g, int depart, int arrivee);
/** @brief Calcule le chemin le plus robuste */
void bellman_ford(Graphe* g, int depart, int arrivee);
/** @brief Recherche par backtracking avec contraintes de BP et Sécurité */
void trouver_chemin_contraint(Graphe* g, int depart, int arrivee, float bp_min, int sec_min);

// Module 3 : Analyse de Sécurité
int est_cyclique_rec(Graphe* g, int v, int* visite, int* pile_rec);
void detecter_cycles(Graphe* g);
void identifier_noeuds_critiques(Graphe* g);

// Module 5 : Fonctions Bonus (Visualisation)
/** @brief Génère un fichier .dot pour visualiser le graphe avec Graphviz */
void generer_dot_graph(Graphe* g, const char* nom_fichier_dot);

#endif
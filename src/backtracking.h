#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include "graphe.h"
#include "liste_chainee.h"

typedef struct Solution {
    int** chemins;
    int* longueurs_chemins;
    int nb_chemins;
    int debit_total;
} Solution;

typedef struct Contraintes {
    int bande_passante_min;
    int cout_max;
    int* nuds_obligatoires;
    int nb_nuds_obligatoires;
    int* nuds_exclus;
    int nb_nuds_exclus;
    int securite_min;
} Contraintes;

typedef struct CheminContraint {
    int* sommets;
    int longueur;
    int latence_totale;
    int cout_total;
    int bande_passante_min;
    int securite_min;
    int respecte_contraintes;
} CheminContraint;

typedef struct Paquet {
    int id;
    int priorite;
    int taille;
    double temps_arrivee;
    double temps_traitement;
    struct Paquet* suivant;
    struct Paquet* precedent;
} Paquet;

typedef struct FilePriorite {
    Paquet* tete;
    Paquet* queue;
    int taille;
    int capacite_max;
    int paquets_perdus;
    int paquets_traites;
    double temps_attente_total;
} FilePriorite;

typedef struct StatistiquesSimulation {
    double temps_attente_moyen;
    double taux_perte;
    double debit_effectif;
    int paquets_envoyes;
    int paquets_recus;
    int paquets_perdus;
    double duree_simulation;
} StatistiquesSimulation;

Solution* trouver_tous_chemins(Graphe* g, int source, int destination);
Solution* optimiser_flux(Graphe* g, int source, int destination, int debit_requis);
void afficher_solution(Solution* sol);
void liberer_solution(Solution* sol);

Contraintes* creer_contraintes();
void liberer_contraintes(Contraintes* contraintes);
CheminContraint* trouver_chemin_avec_contraintes(Graphe* g, int source, int destination, Contraintes* contraintes);
void afficher_chemin_contraint(CheminContraint* chemin);
void liberer_chemin_contraint(CheminContraint* chemin);

FilePriorite* creer_file_priorite(int capacite_max);
int enqueue(FilePriorite* file, int id, int priorite, int taille, double temps_arrivee);
Paquet* dequeue(FilePriorite* file);
Paquet* peek(FilePriorite* file);
int est_vide_file(FilePriorite* file);
int est_pleine_file(FilePriorite* file);
void afficher_file(FilePriorite* file);
void liberer_file_priorite(FilePriorite* file);

StatistiquesSimulation* simuler_flux_paquets(Graphe* g, int source, int destination, int nb_paquets, int capacite_file);
void afficher_statistiques_simulation(StatistiquesSimulation* stats);
void liberer_statistiques(StatistiquesSimulation* stats);

#endif

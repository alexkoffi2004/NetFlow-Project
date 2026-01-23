#ifndef SECURITE_H
#define SECURITE_H

#include "graphe.h"
#include "liste_chainee.h"

typedef struct AnalyseSecurite {
    int* points_critiques;
    int nb_points_critiques;
    int** chemins_redondants;
    int nb_chemins_redondants;
    double fiabilite_globale;
} AnalyseSecurite;

typedef struct CycleInfo {
    int** cycles;
    int* longueurs_cycles;
    int nb_cycles;
    int a_cycles;
} CycleInfo;

typedef struct ComposanteFC {
    int** composantes;
    int* tailles;
    int nb_composantes;
} ComposanteFC;

AnalyseSecurite* analyser_securite(Graphe* g);
int* identifier_points_critiques(Graphe* g, int* nb_points);
int** identifier_ponts(Graphe* g, int* nb_ponts);
int verifier_connectivite(Graphe* g, int source, int destination);
double calculer_fiabilite(Graphe* g, int source, int destination);
void afficher_analyse_securite(AnalyseSecurite* analyse);
void liberer_analyse_securite(AnalyseSecurite* analyse);

CycleInfo* detecter_cycles_dfs(Graphe* g);
CycleInfo* detecter_cycles_bfs(Graphe* g);
void afficher_cycles(CycleInfo* info);
void liberer_cycles(CycleInfo* info);

ComposanteFC* tarjan_composantes_fortement_connexes(Graphe* g);
void afficher_composantes_fc(ComposanteFC* comp);
void liberer_composantes_fc(ComposanteFC* comp);
int* identifier_sous_reseaux_isoles(Graphe* g, int* nb_sous_reseaux);

#endif

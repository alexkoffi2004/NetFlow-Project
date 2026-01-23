#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/graphe.h"
#include "../src/liste_chainee.h"
#include "../src/dijkstra.h"
#include "../src/backtracking.h"
#include "../src/securite.h"

void test_creation_graphe() {
    printf("Test: Creation de graphe... ");
    Graphe* g = creer_graphe(5);
    assert(g != NULL);
    assert(g->nb_sommets == 5);
    liberer_graphe(g);
    printf("OK\n");
}

void test_ajout_arete() {
    printf("Test: Ajout d'aretes... ");
    Graphe* g = creer_graphe(3);
    ajouter_arete(g, 0, 1, 10, 5);
    ajouter_arete(g, 1, 2, 15, 3);
    assert(g->sommets[0].aretes != NULL);
    assert(g->sommets[0].aretes->destination == 1);
    assert(g->sommets[0].aretes->capacite == 10);
    liberer_graphe(g);
    printf("OK\n");
}

void test_liste_chainee() {
    printf("Test: Liste chainee... ");
    Liste* liste = creer_liste();
    assert(liste != NULL);
    assert(est_vide(liste) == 1);
    
    ajouter_debut(liste, 10);
    ajouter_fin(liste, 20);
    assert(liste->taille == 2);
    assert(contient(liste, 10) == 1);
    assert(contient(liste, 30) == 0);
    
    int val = supprimer_debut(liste);
    assert(val == 10);
    assert(liste->taille == 1);
    
    liberer_liste(liste);
    printf("OK\n");
}

void test_dijkstra() {
    printf("Test: Algorithme de Dijkstra... ");
    Graphe* g = creer_graphe(4);
    ajouter_arete(g, 0, 1, 10, 1);
    ajouter_arete(g, 0, 2, 10, 4);
    ajouter_arete(g, 1, 2, 10, 2);
    ajouter_arete(g, 1, 3, 10, 5);
    ajouter_arete(g, 2, 3, 10, 1);
    
    CheminResult* resultat = dijkstra(g, 0, 3);
    assert(resultat != NULL);
    assert(resultat->cout_total == 4);
    
    liberer_chemin(resultat);
    liberer_graphe(g);
    printf("OK\n");
}

void test_backtracking() {
    printf("Test: Backtracking... ");
    Graphe* g = creer_graphe(4);
    ajouter_arete(g, 0, 1, 10, 1);
    ajouter_arete(g, 0, 2, 10, 1);
    ajouter_arete(g, 1, 3, 10, 1);
    ajouter_arete(g, 2, 3, 10, 1);
    
    Solution* sol = trouver_tous_chemins(g, 0, 3);
    assert(sol != NULL);
    assert(sol->nb_chemins >= 2);
    
    liberer_solution(sol);
    liberer_graphe(g);
    printf("OK\n");
}

void test_connectivite() {
    printf("Test: Verification de connectivite... ");
    Graphe* g = creer_graphe(4);
    ajouter_arete(g, 0, 1, 10, 1);
    ajouter_arete(g, 1, 2, 10, 1);
    
    assert(verifier_connectivite(g, 0, 2) == 1);
    assert(verifier_connectivite(g, 0, 3) == 0);
    
    liberer_graphe(g);
    printf("OK\n");
}

void test_chargement_fichier() {
    printf("Test: Chargement depuis fichier... ");
    Graphe* g = charger_graphe_fichier("../data/reseau_test1.txt");
    if (g != NULL) {
        assert(g->nb_sommets == 5);
        liberer_graphe(g);
        printf("OK\n");
    } else {
        printf("SKIP (fichier non trouve)\n");
    }
}

void test_securite() {
    printf("Test: Analyse de securite... ");
    Graphe* g = creer_graphe(5);
    ajouter_arete(g, 0, 1, 10, 1);
    ajouter_arete(g, 1, 2, 10, 1);
    ajouter_arete(g, 2, 3, 10, 1);
    ajouter_arete(g, 3, 4, 10, 1);
    
    AnalyseSecurite* analyse = analyser_securite(g);
    assert(analyse != NULL);
    
    liberer_analyse_securite(analyse);
    liberer_graphe(g);
    printf("OK\n");
}

int main() {
    printf("\n========================================\n");
    printf("   TESTS UNITAIRES - PROJET NETFLOW    \n");
    printf("========================================\n\n");
    
    test_creation_graphe();
    test_ajout_arete();
    test_liste_chainee();
    test_dijkstra();
    test_backtracking();
    test_connectivite();
    test_chargement_fichier();
    test_securite();
    
    printf("\n========================================\n");
    printf("   TOUS LES TESTS SONT PASSES!         \n");
    printf("========================================\n\n");
    
    return 0;
}

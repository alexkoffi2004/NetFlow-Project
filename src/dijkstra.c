#include "dijkstra.h"
#include <limits.h>
#include <string.h>

#define INFINI INT_MAX

static int trouver_min_distance(int* distances, int* visite, int nb_sommets) {
    int min = INFINI;
    int min_index = -1;
    
    for (int i = 0; i < nb_sommets; i++) {
        if (!visite[i] && distances[i] < min) {
            min = distances[i];
            min_index = i;
        }
    }
    
    return min_index;
}

CheminResult* dijkstra(Graphe* g, int source, int destination) {
    if (!g || source < 0 || source >= g->nb_sommets || 
        destination < 0 || destination >= g->nb_sommets) {
        return NULL;
    }
    
    CheminResult* resultat = (CheminResult*)malloc(sizeof(CheminResult));
    if (!resultat) return NULL;
    
    resultat->distances = (int*)malloc(g->nb_sommets * sizeof(int));
    resultat->predecesseurs = (int*)malloc(g->nb_sommets * sizeof(int));
    int* visite = (int*)calloc(g->nb_sommets, sizeof(int));
    
    if (!resultat->distances || !resultat->predecesseurs || !visite) {
        free(resultat->distances);
        free(resultat->predecesseurs);
        free(visite);
        free(resultat);
        return NULL;
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        resultat->distances[i] = INFINI;
        resultat->predecesseurs[i] = -1;
    }
    
    resultat->distances[source] = 0;
    
    for (int count = 0; count < g->nb_sommets - 1; count++) {
        int u = trouver_min_distance(resultat->distances, visite, g->nb_sommets);
        
        if (u == -1 || resultat->distances[u] == INFINI) break;
        
        visite[u] = 1;
        
        Arete* arete = g->sommets[u].aretes;
        while (arete) {
            int v = arete->destination;
            if (!visite[v] && resultat->distances[u] != INFINI) {
                int nouvelle_distance = resultat->distances[u] + arete->cout;
                if (nouvelle_distance < resultat->distances[v]) {
                    resultat->distances[v] = nouvelle_distance;
                    resultat->predecesseurs[v] = u;
                }
            }
            arete = arete->suivant;
        }
    }
    
    resultat->cout_total = resultat->distances[destination];
    free(visite);
    
    return resultat;
}

void afficher_chemin(CheminResult* resultat, int source, int destination) {
    if (!resultat) return;
    
    if (resultat->distances[destination] == INFINI) {
        printf("Aucun chemin trouve entre %d et %d\n", source, destination);
        return;
    }
    
    printf("\nChemin le plus court de %d a %d:\n", source, destination);
    printf("Cout total: %d\n", resultat->cout_total);
    printf("Chemin: ");
    
    int longueur;
    int* chemin = reconstruire_chemin(resultat, source, destination, &longueur);
    
    if (chemin) {
        for (int i = 0; i < longueur; i++) {
            printf("%d", chemin[i]);
            if (i < longueur - 1) printf(" -> ");
        }
        printf("\n");
        free(chemin);
    }
}

void liberer_chemin(CheminResult* resultat) {
    if (!resultat) return;
    
    free(resultat->distances);
    free(resultat->predecesseurs);
    free(resultat);
}

int* reconstruire_chemin(CheminResult* resultat, int source, int destination, int* longueur) {
    if (!resultat || resultat->distances[destination] == INFINI) {
        *longueur = 0;
        return NULL;
    }
    
    int* chemin_inverse = (int*)malloc(resultat->cout_total * sizeof(int));
    if (!chemin_inverse) {
        *longueur = 0;
        return NULL;
    }
    
    int count = 0;
    int courant = destination;
    
    while (courant != -1) {
        chemin_inverse[count++] = courant;
        courant = resultat->predecesseurs[courant];
    }
    
    int* chemin = (int*)malloc(count * sizeof(int));
    if (!chemin) {
        free(chemin_inverse);
        *longueur = 0;
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        chemin[i] = chemin_inverse[count - 1 - i];
    }
    
    *longueur = count;
    free(chemin_inverse);
    
    return chemin;
}

CheminResult* bellman_ford(Graphe* g, int source, int destination) {
    if (!g || source < 0 || source >= g->nb_sommets || 
        destination < 0 || destination >= g->nb_sommets) {
        return NULL;
    }
    
    CheminResult* resultat = (CheminResult*)malloc(sizeof(CheminResult));
    if (!resultat) return NULL;
    
    resultat->distances = (int*)malloc(g->nb_sommets * sizeof(int));
    resultat->predecesseurs = (int*)malloc(g->nb_sommets * sizeof(int));
    
    if (!resultat->distances || !resultat->predecesseurs) {
        free(resultat->distances);
        free(resultat->predecesseurs);
        free(resultat);
        return NULL;
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        resultat->distances[i] = INFINI;
        resultat->predecesseurs[i] = -1;
    }
    
    resultat->distances[source] = 0;
    resultat->a_cycle_negatif = 0;
    
    for (int i = 0; i < g->nb_sommets - 1; i++) {
        for (int u = 0; u < g->nb_sommets; u++) {
            if (resultat->distances[u] == INFINI) continue;
            
            Arete* arete = g->sommets[u].aretes;
            while (arete) {
                int v = arete->destination;
                int poids = arete->latence;
                
                if (resultat->distances[u] != INFINI && 
                    resultat->distances[u] + poids < resultat->distances[v]) {
                    resultat->distances[v] = resultat->distances[u] + poids;
                    resultat->predecesseurs[v] = u;
                }
                arete = arete->suivant;
            }
        }
    }
    
    for (int u = 0; u < g->nb_sommets; u++) {
        if (resultat->distances[u] == INFINI) continue;
        
        Arete* arete = g->sommets[u].aretes;
        while (arete) {
            int v = arete->destination;
            int poids = arete->latence;
            
            if (resultat->distances[u] != INFINI && 
                resultat->distances[u] + poids < resultat->distances[v]) {
                resultat->a_cycle_negatif = 1;
                break;
            }
            arete = arete->suivant;
        }
        if (resultat->a_cycle_negatif) break;
    }
    
    resultat->cout_total = resultat->distances[destination];
    return resultat;
}

static int comparer_chemins(const void* a, const void* b) {
    CheminK* c1 = (CheminK*)a;
    CheminK* c2 = (CheminK*)b;
    return c1->cout_total - c2->cout_total;
}

static int chemin_existe(CheminK* chemins, int nb_chemins, int* nouveau_chemin, int longueur) {
    for (int i = 0; i < nb_chemins; i++) {
        if (chemins[i].longueur == longueur) {
            int identique = 1;
            for (int j = 0; j < longueur; j++) {
                if (chemins[i].sommets[j] != nouveau_chemin[j]) {
                    identique = 0;
                    break;
                }
            }
            if (identique) return 1;
        }
    }
    return 0;
}

static void trouver_chemins_recursif(Graphe* g, int courant, int destination,
                                     int* visite, int* chemin_actuel, int profondeur,
                                     CheminK** chemins, int* nb_chemins, int k,
                                     int cout_actuel) {
    if (*nb_chemins >= k) return;
    
    visite[courant] = 1;
    chemin_actuel[profondeur] = courant;
    
    if (courant == destination) {
        if (!chemin_existe(*chemins, *nb_chemins, chemin_actuel, profondeur + 1)) {
            *chemins = (CheminK*)realloc(*chemins, (*nb_chemins + 1) * sizeof(CheminK));
            
            (*chemins)[*nb_chemins].sommets = (int*)malloc((profondeur + 1) * sizeof(int));
            memcpy((*chemins)[*nb_chemins].sommets, chemin_actuel, (profondeur + 1) * sizeof(int));
            (*chemins)[*nb_chemins].longueur = profondeur + 1;
            (*chemins)[*nb_chemins].cout_total = cout_actuel;
            (*nb_chemins)++;
            
            qsort(*chemins, *nb_chemins, sizeof(CheminK), comparer_chemins);
            
            if (*nb_chemins > k) {
                free((*chemins)[k].sommets);
                *nb_chemins = k;
            }
        }
    } else {
        Arete* arete = g->sommets[courant].aretes;
        while (arete && *nb_chemins < k * 2) {
            if (!visite[arete->destination]) {
                trouver_chemins_recursif(g, arete->destination, destination, visite,
                                        chemin_actuel, profondeur + 1, chemins, nb_chemins,
                                        k, cout_actuel + arete->latence);
            }
            arete = arete->suivant;
        }
    }
    
    visite[courant] = 0;
}

KCheminsResult* k_plus_courts_chemins(Graphe* g, int source, int destination, int k) {
    if (!g || source < 0 || source >= g->nb_sommets || 
        destination < 0 || destination >= g->nb_sommets || k <= 0) {
        return NULL;
    }
    
    KCheminsResult* resultat = (KCheminsResult*)malloc(sizeof(KCheminsResult));
    if (!resultat) return NULL;
    
    resultat->chemins = NULL;
    resultat->nb_chemins = 0;
    resultat->k = k;
    
    int* visite = (int*)calloc(g->nb_sommets, sizeof(int));
    int* chemin_actuel = (int*)malloc(g->nb_sommets * sizeof(int));
    
    if (!visite || !chemin_actuel) {
        free(visite);
        free(chemin_actuel);
        free(resultat);
        return NULL;
    }
    
    trouver_chemins_recursif(g, source, destination, visite, chemin_actuel, 0,
                            &resultat->chemins, &resultat->nb_chemins, k, 0);
    
    free(visite);
    free(chemin_actuel);
    
    return resultat;
}

void afficher_k_chemins(KCheminsResult* resultat) {
    if (!resultat) return;
    
    printf("\n=== %d Plus Courts Chemins ===\n", resultat->k);
    printf("Nombre de chemins trouves: %d\n\n", resultat->nb_chemins);
    
    for (int i = 0; i < resultat->nb_chemins; i++) {
        printf("Chemin %d (cout: %d): ", i + 1, resultat->chemins[i].cout_total);
        for (int j = 0; j < resultat->chemins[i].longueur; j++) {
            printf("%d", resultat->chemins[i].sommets[j]);
            if (j < resultat->chemins[i].longueur - 1) printf(" -> ");
        }
        printf("\n");
    }
    
    double redondance = analyser_redondance(resultat);
    printf("\nTaux de redondance: %.2f%%\n", redondance * 100);
}

void liberer_k_chemins(KCheminsResult* resultat) {
    if (!resultat) return;
    
    for (int i = 0; i < resultat->nb_chemins; i++) {
        free(resultat->chemins[i].sommets);
    }
    free(resultat->chemins);
    free(resultat);
}

double analyser_redondance(KCheminsResult* resultat) {
    if (!resultat || resultat->nb_chemins == 0) return 0.0;
    
    int aretes_uniques = 0;
    int aretes_totales = 0;
    
    for (int i = 0; i < resultat->nb_chemins; i++) {
        aretes_totales += resultat->chemins[i].longueur - 1;
    }
    
    if (aretes_totales == 0) return 0.0;
    
    int** aretes_vues = (int**)malloc(aretes_totales * sizeof(int*));
    for (int i = 0; i < aretes_totales; i++) {
        aretes_vues[i] = (int*)malloc(2 * sizeof(int));
    }
    
    int count = 0;
    for (int i = 0; i < resultat->nb_chemins; i++) {
        for (int j = 0; j < resultat->chemins[i].longueur - 1; j++) {
            int u = resultat->chemins[i].sommets[j];
            int v = resultat->chemins[i].sommets[j + 1];
            
            int existe = 0;
            for (int k = 0; k < count; k++) {
                if (aretes_vues[k][0] == u && aretes_vues[k][1] == v) {
                    existe = 1;
                    break;
                }
            }
            
            if (!existe) {
                aretes_vues[count][0] = u;
                aretes_vues[count][1] = v;
                count++;
                aretes_uniques++;
            }
        }
    }
    
    for (int i = 0; i < aretes_totales; i++) {
        free(aretes_vues[i]);
    }
    free(aretes_vues);
    
    return 1.0 - ((double)aretes_uniques / aretes_totales);
}

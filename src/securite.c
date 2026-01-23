#include "securite.h"
#include "dijkstra.h"
#include <string.h>
#include <limits.h>

static int est_point_critique(Graphe* g, int sommet, int source, int destination) {
    if (sommet == source || sommet == destination) return 0;
    
    Graphe* g_temp = creer_graphe(g->nb_sommets);
    if (!g_temp) return 0;
    
    for (int i = 0; i < g->nb_sommets; i++) {
        if (i == sommet) continue;
        
        Arete* arete = g->sommets[i].aretes;
        while (arete) {
            if (arete->destination != sommet) {
                ajouter_arete(g_temp, i, arete->destination, arete->latence, arete->bande_passante, arete->cout, arete->securite);
            }
            arete = arete->suivant;
        }
    }
    
    int connecte = verifier_connectivite(g_temp, source, destination);
    liberer_graphe(g_temp);
    
    return !connecte;
}

static int min(int a, int b) {
    return (a < b) ? a : b;
}

static void dfs_articulation(Graphe* g, int u, int* visite, int* disc, int* low,
                             int* parent, int* est_articulation, int* temps) {
    int enfants = 0;
    visite[u] = 1;
    disc[u] = low[u] = ++(*temps);
    
    Arete* arete = g->sommets[u].aretes;
    while (arete) {
        int v = arete->destination;
        
        if (!visite[v]) {
            enfants++;
            parent[v] = u;
            dfs_articulation(g, v, visite, disc, low, parent, est_articulation, temps);
            
            if (low[v] < low[u]) {
                low[u] = low[v];
            }
            
            if (parent[u] == -1 && enfants > 1) {
                est_articulation[u] = 1;
            }
            
            if (parent[u] != -1 && low[v] >= disc[u]) {
                est_articulation[u] = 1;
            }
        } else if (v != parent[u]) {
            if (disc[v] < low[u]) {
                low[u] = disc[v];
            }
        }
        arete = arete->suivant;
    }
}

int* identifier_points_critiques(Graphe* g, int* nb_points) {
    if (!g || !nb_points) return NULL;
    
    *nb_points = 0;
    int* visite = (int*)calloc(g->nb_sommets, sizeof(int));
    int* disc = (int*)malloc(g->nb_sommets * sizeof(int));
    int* low = (int*)malloc(g->nb_sommets * sizeof(int));
    int* parent = (int*)malloc(g->nb_sommets * sizeof(int));
    int* est_articulation = (int*)calloc(g->nb_sommets, sizeof(int));
    
    if (!visite || !disc || !low || !parent || !est_articulation) {
        free(visite);
        free(disc);
        free(low);
        free(parent);
        free(est_articulation);
        return NULL;
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        parent[i] = -1;
    }
    
    int temps = 0;
    for (int i = 0; i < g->nb_sommets; i++) {
        if (!visite[i]) {
            dfs_articulation(g, i, visite, disc, low, parent, est_articulation, &temps);
        }
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        if (est_articulation[i]) {
            (*nb_points)++;
        }
    }
    
    int* points = NULL;
    if (*nb_points > 0) {
        points = (int*)malloc(*nb_points * sizeof(int));
        int index = 0;
        for (int i = 0; i < g->nb_sommets; i++) {
            if (est_articulation[i]) {
                points[index++] = i;
            }
        }
    }
    
    free(visite);
    free(disc);
    free(low);
    free(parent);
    free(est_articulation);
    
    return points;
}

int** identifier_ponts(Graphe* g, int* nb_ponts) {
    if (!g || !nb_ponts) return NULL;
    *nb_ponts = 0;
    return NULL;
}

int verifier_connectivite(Graphe* g, int source, int destination) {
    if (!g || source < 0 || source >= g->nb_sommets || 
        destination < 0 || destination >= g->nb_sommets) {
        return 0;
    }
    
    CheminResult* resultat = dijkstra(g, source, destination);
    if (!resultat) return 0;
    
    int connecte = (resultat->distances[destination] != INT_MAX);
    liberer_chemin(resultat);
    
    return connecte;
}

double calculer_fiabilite(Graphe* g, int source, int destination) {
    if (!g) return 0.0;
    
    int nb_points_critiques;
    int* points = identifier_points_critiques(g, &nb_points_critiques);
    
    double fiabilite = 1.0;
    
    if (nb_points_critiques > 0) {
        fiabilite = 1.0 / (1.0 + nb_points_critiques * 0.2);
    }
    
    int nb_aretes_totales = 0;
    for (int i = 0; i < g->nb_sommets; i++) {
        Arete* arete = g->sommets[i].aretes;
        while (arete) {
            nb_aretes_totales++;
            arete = arete->suivant;
        }
    }
    
    double densite = (double)nb_aretes_totales / (g->nb_sommets * (g->nb_sommets - 1));
    fiabilite *= (0.5 + 0.5 * densite);
    
    free(points);
    return fiabilite;
}

AnalyseSecurite* analyser_securite(Graphe* g) {
    if (!g) return NULL;
    
    AnalyseSecurite* analyse = (AnalyseSecurite*)malloc(sizeof(AnalyseSecurite));
    if (!analyse) return NULL;
    
    analyse->points_critiques = identifier_points_critiques(g, &analyse->nb_points_critiques);
    analyse->chemins_redondants = NULL;
    analyse->nb_chemins_redondants = 0;
    analyse->fiabilite_globale = calculer_fiabilite(g, 0, g->nb_sommets - 1);
    
    return analyse;
}

void afficher_analyse_securite(AnalyseSecurite* analyse) {
    if (!analyse) return;
    
    printf("\n=== Analyse de securite ===\n");
    printf("Fiabilite globale: %.2f%%\n", analyse->fiabilite_globale * 100);
    printf("Nombre de points critiques: %d\n", analyse->nb_points_critiques);
    
    if (analyse->nb_points_critiques > 0) {
        printf("Points critiques identifies: ");
        for (int i = 0; i < analyse->nb_points_critiques; i++) {
            printf("%d ", analyse->points_critiques[i]);
        }
        printf("\n");
    }
    
    if (analyse->fiabilite_globale < 0.5) {
        printf("ALERTE: Fiabilite faible - reseau vulnerable!\n");
    } else if (analyse->fiabilite_globale < 0.7) {
        printf("ATTENTION: Fiabilite moyenne - ameliorations recommandees\n");
    } else {
        printf("OK: Bonne fiabilite du reseau\n");
    }
}

void liberer_analyse_securite(AnalyseSecurite* analyse) {
    if (!analyse) return;
    
    free(analyse->points_critiques);
    
    if (analyse->chemins_redondants) {
        for (int i = 0; i < analyse->nb_chemins_redondants; i++) {
            free(analyse->chemins_redondants[i]);
        }
        free(analyse->chemins_redondants);
    }
    
    free(analyse);
}

static int dfs_cycle_util(Graphe* g, int sommet, int* visite, int* pile_recursion,
                          int* chemin, int profondeur, CycleInfo* info) {
    visite[sommet] = 1;
    pile_recursion[sommet] = 1;
    chemin[profondeur] = sommet;
    
    Arete* arete = g->sommets[sommet].aretes;
    while (arete) {
        int voisin = arete->destination;
        
        if (!visite[voisin]) {
            if (dfs_cycle_util(g, voisin, visite, pile_recursion, chemin, profondeur + 1, info)) {
                return 1;
            }
        } else if (pile_recursion[voisin]) {
            info->a_cycles = 1;
            
            int debut_cycle = profondeur;
            while (debut_cycle >= 0 && chemin[debut_cycle] != voisin) {
                debut_cycle--;
            }
            
            if (debut_cycle >= 0) {
                int longueur = profondeur - debut_cycle + 1;
                info->cycles = (int**)realloc(info->cycles, (info->nb_cycles + 1) * sizeof(int*));
                info->longueurs_cycles = (int*)realloc(info->longueurs_cycles, 
                                                       (info->nb_cycles + 1) * sizeof(int));
                
                info->cycles[info->nb_cycles] = (int*)malloc(longueur * sizeof(int));
                for (int i = 0; i < longueur; i++) {
                    info->cycles[info->nb_cycles][i] = chemin[debut_cycle + i];
                }
                info->longueurs_cycles[info->nb_cycles] = longueur;
                info->nb_cycles++;
            }
        }
        arete = arete->suivant;
    }
    
    pile_recursion[sommet] = 0;
    return 0;
}

CycleInfo* detecter_cycles_dfs(Graphe* g) {
    if (!g) return NULL;
    
    CycleInfo* info = (CycleInfo*)malloc(sizeof(CycleInfo));
    if (!info) return NULL;
    
    info->cycles = NULL;
    info->longueurs_cycles = NULL;
    info->nb_cycles = 0;
    info->a_cycles = 0;
    
    int* visite = (int*)calloc(g->nb_sommets, sizeof(int));
    int* pile_recursion = (int*)calloc(g->nb_sommets, sizeof(int));
    int* chemin = (int*)malloc(g->nb_sommets * sizeof(int));
    
    if (!visite || !pile_recursion || !chemin) {
        free(visite);
        free(pile_recursion);
        free(chemin);
        free(info);
        return NULL;
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        if (!visite[i]) {
            dfs_cycle_util(g, i, visite, pile_recursion, chemin, 0, info);
        }
    }
    
    free(visite);
    free(pile_recursion);
    free(chemin);
    
    return info;
}

CycleInfo* detecter_cycles_bfs(Graphe* g) {
    if (!g) return NULL;
    
    CycleInfo* info = (CycleInfo*)malloc(sizeof(CycleInfo));
    if (!info) return NULL;
    
    info->cycles = NULL;
    info->longueurs_cycles = NULL;
    info->nb_cycles = 0;
    info->a_cycles = 0;
    
    int* degre_entrant = (int*)calloc(g->nb_sommets, sizeof(int));
    if (!degre_entrant) {
        free(info);
        return NULL;
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        Arete* arete = g->sommets[i].aretes;
        while (arete) {
            degre_entrant[arete->destination]++;
            arete = arete->suivant;
        }
    }
    
    Liste* file = creer_liste();
    for (int i = 0; i < g->nb_sommets; i++) {
        if (degre_entrant[i] == 0) {
            ajouter_fin(file, i);
        }
    }
    
    int count = 0;
    while (!est_vide(file)) {
        int u = supprimer_debut(file);
        count++;
        
        Arete* arete = g->sommets[u].aretes;
        while (arete) {
            degre_entrant[arete->destination]--;
            if (degre_entrant[arete->destination] == 0) {
                ajouter_fin(file, arete->destination);
            }
            arete = arete->suivant;
        }
    }
    
    if (count != g->nb_sommets) {
        info->a_cycles = 1;
    }
    
    liberer_liste(file);
    free(degre_entrant);
    
    return info;
}

void afficher_cycles(CycleInfo* info) {
    if (!info) return;
    
    printf("\n=== Detection de Cycles ===\n");
    if (info->a_cycles) {
        printf("ALERTE: Cycles detectes dans le graphe!\n");
        printf("Nombre de cycles: %d\n\n", info->nb_cycles);
        
        for (int i = 0; i < info->nb_cycles; i++) {
            printf("Cycle %d: ", i + 1);
            for (int j = 0; j < info->longueurs_cycles[i]; j++) {
                printf("%d", info->cycles[i][j]);
                if (j < info->longueurs_cycles[i] - 1) printf(" -> ");
            }
            printf("\n");
        }
    } else {
        printf("Aucun cycle detecte - Graphe acyclique\n");
    }
}

void liberer_cycles(CycleInfo* info) {
    if (!info) return;
    
    for (int i = 0; i < info->nb_cycles; i++) {
        free(info->cycles[i]);
    }
    free(info->cycles);
    free(info->longueurs_cycles);
    free(info);
}

static void tarjan_dfs(Graphe* g, int u, int* disc, int* low, int* sur_pile,
                       Liste* pile, int* temps, ComposanteFC* result) {
    disc[u] = low[u] = ++(*temps);
    ajouter_debut(pile, u);
    sur_pile[u] = 1;
    
    Arete* arete = g->sommets[u].aretes;
    while (arete) {
        int v = arete->destination;
        
        if (disc[v] == -1) {
            tarjan_dfs(g, v, disc, low, sur_pile, pile, temps, result);
            low[u] = min(low[u], low[v]);
        } else if (sur_pile[v]) {
            low[u] = min(low[u], disc[v]);
        }
        arete = arete->suivant;
    }
    
    if (low[u] == disc[u]) {
        int* composante = (int*)malloc(g->nb_sommets * sizeof(int));
        int taille = 0;
        int w;
        
        do {
            w = supprimer_debut(pile);
            sur_pile[w] = 0;
            composante[taille++] = w;
        } while (w != u);
        
        result->composantes = (int**)realloc(result->composantes, 
                                             (result->nb_composantes + 1) * sizeof(int*));
        result->tailles = (int*)realloc(result->tailles, 
                                        (result->nb_composantes + 1) * sizeof(int));
        
        result->composantes[result->nb_composantes] = (int*)malloc(taille * sizeof(int));
        memcpy(result->composantes[result->nb_composantes], composante, taille * sizeof(int));
        result->tailles[result->nb_composantes] = taille;
        result->nb_composantes++;
        
        free(composante);
    }
}

ComposanteFC* tarjan_composantes_fortement_connexes(Graphe* g) {
    if (!g) return NULL;
    
    ComposanteFC* result = (ComposanteFC*)malloc(sizeof(ComposanteFC));
    if (!result) return NULL;
    
    result->composantes = NULL;
    result->tailles = NULL;
    result->nb_composantes = 0;
    
    int* disc = (int*)malloc(g->nb_sommets * sizeof(int));
    int* low = (int*)malloc(g->nb_sommets * sizeof(int));
    int* sur_pile = (int*)calloc(g->nb_sommets, sizeof(int));
    Liste* pile = creer_liste();
    
    if (!disc || !low || !sur_pile || !pile) {
        free(disc);
        free(low);
        free(sur_pile);
        if (pile) liberer_liste(pile);
        free(result);
        return NULL;
    }
    
    for (int i = 0; i < g->nb_sommets; i++) {
        disc[i] = -1;
        low[i] = -1;
    }
    
    int temps = 0;
    for (int i = 0; i < g->nb_sommets; i++) {
        if (disc[i] == -1) {
            tarjan_dfs(g, i, disc, low, sur_pile, pile, &temps, result);
        }
    }
    
    free(disc);
    free(low);
    free(sur_pile);
    liberer_liste(pile);
    
    return result;
}

void afficher_composantes_fc(ComposanteFC* comp) {
    if (!comp) return;
    
    printf("\n=== Composantes Fortement Connexes (Tarjan) ===\n");
    printf("Nombre de composantes: %d\n\n", comp->nb_composantes);
    
    for (int i = 0; i < comp->nb_composantes; i++) {
        printf("Composante %d (taille: %d): ", i + 1, comp->tailles[i]);
        for (int j = 0; j < comp->tailles[i]; j++) {
            printf("%d", comp->composantes[i][j]);
            if (j < comp->tailles[i] - 1) printf(", ");
        }
        printf("\n");
    }
    
    if (comp->nb_composantes > 1) {
        printf("\n[INFO] Reseau fragmente en %d sous-reseaux\n", comp->nb_composantes);
    } else {
        printf("\n[OK] Reseau fortement connexe\n");
    }
}

void liberer_composantes_fc(ComposanteFC* comp) {
    if (!comp) return;
    
    for (int i = 0; i < comp->nb_composantes; i++) {
        free(comp->composantes[i]);
    }
    free(comp->composantes);
    free(comp->tailles);
    free(comp);
}

int* identifier_sous_reseaux_isoles(Graphe* g, int* nb_sous_reseaux) {
    if (!g || !nb_sous_reseaux) return NULL;
    
    ComposanteFC* comp = tarjan_composantes_fortement_connexes(g);
    if (!comp) return NULL;
    
    *nb_sous_reseaux = comp->nb_composantes;
    
    int* sous_reseaux = NULL;
    if (comp->nb_composantes > 1) {
        sous_reseaux = (int*)malloc(comp->nb_composantes * sizeof(int));
        for (int i = 0; i < comp->nb_composantes; i++) {
            sous_reseaux[i] = comp->tailles[i];
        }
    }
    
    liberer_composantes_fc(comp);
    return sous_reseaux;
}

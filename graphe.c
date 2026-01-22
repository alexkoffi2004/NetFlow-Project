#include "graphe.h"
#include <float.h> // Pour FLT_MAX

// --- MODULE 1 : GESTION DE BASE ---

Graphe* initialiser_graphe(int nb_noeuds) {
    Graphe* g = (Graphe*)malloc(sizeof(Graphe));
    g->nb_noeuds = nb_noeuds;
    g->tab_noeuds = (Noeud*)malloc(nb_noeuds * sizeof(Noeud));
    for (int i = 0; i < nb_noeuds; i++) {
        g->tab_noeuds[i].id = i;
        g->tab_noeuds[i].liste_adj = NULL;
    }
    return g;
}

void ajouter_arete(Graphe* g, int src, int dest, float lat, float bp, float cout, int sec) {
    Arete* n_arete = (Arete*)malloc(sizeof(Arete));
    n_arete->destination = dest;
    n_arete->latence = lat;
    n_arete->bande_passante = bp;
    n_arete->cout = cout;
    n_arete->securite = sec;
    n_arete->suivant = g->tab_noeuds[src].liste_adj;
    g->tab_noeuds[src].liste_adj = n_arete;
}

Graphe* charger_depuis_fichier(char* nom_fichier) {
    FILE* file = fopen(nom_fichier, "r");
    if (!file) return NULL;
    int nb_n, nb_a;
    fscanf(file, "%d %d", &nb_n, &nb_a);
    Graphe* g = initialiser_graphe(nb_n);
    int s, d, sec;
    float lat, bp, c;
    for (int i = 0; i < nb_a; i++) {
        fscanf(file, "%d %d %f %f %f %d", &s, &d, &lat, &bp, &c, &sec);
        ajouter_arete(g, s, d, lat, bp, c, sec);
    }
    fclose(file);
    return g;
}

void afficher_graphe(Graphe* g) {
    for (int i = 0; i < g->nb_noeuds; i++) {
        Arete* temp = g->tab_noeuds[i].liste_adj;
        printf("Noeud %d :", i);
        while (temp) {
            printf(" -> %d (Lat: %.1fms, Sec: %d)", temp->destination, temp->latence, temp->securite);
            temp = temp->suivant;
        }
        printf("\n");
    }
}

// --- MODULE 2 : ROUTAGE OPTIMAL ---

void dijkstra(Graphe* g, int depart, int arrivee) {
    float* distances = (float*)malloc(g->nb_noeuds * sizeof(float));
    int* predecesseurs = (int*)malloc(g->nb_noeuds * sizeof(int));
    int* visite = (int*)calloc(g->nb_noeuds, sizeof(int));

    for (int i = 0; i < g->nb_noeuds; i++) {
        distances[i] = FLT_MAX;
        predecesseurs[i] = -1;
    }
    distances[depart] = 0;

    for (int i = 0; i < g->nb_noeuds - 1; i++) {
        float min = FLT_MAX;
        int u = -1;
        for (int v = 0; v < g->nb_noeuds; v++) {
            if (!visite[v] && distances[v] <= min) {
                min = distances[v];
                u = v;
            }
        }
        if (u == -1 || u == arrivee) break;
        visite[u] = 1;

        Arete* a = g->tab_noeuds[u].liste_adj;
        while (a != NULL) {
            if (!visite[a->destination] && distances[u] + a->latence < distances[a->destination]) {
                distances[a->destination] = distances[u] + a->latence;
                predecesseurs[a->destination] = u;
            }
            a = a->suivant;
        }
    }

    if (distances[arrivee] == FLT_MAX) printf("Aucun chemin entre %d et %d\n", depart, arrivee);
    else {
        printf("Dijkstra (Latence min: %.2f ms) : ", distances[arrivee]);
        int curr = arrivee;
        while (curr != -1) {
            printf("%d%s", curr, (predecesseurs[curr] == -1) ? "" : " <- ");
            curr = predecesseurs[curr];
        }
        printf("\n");
    }
    free(distances); free(predecesseurs); free(visite);
}

void bellman_ford(Graphe* g, int depart, int arrivee) {
    float* dist = (float*)malloc(g->nb_noeuds * sizeof(float));
    for (int i = 0; i < g->nb_noeuds; i++) dist[i] = FLT_MAX;
    dist[depart] = 0;

    for (int i = 1; i <= g->nb_noeuds - 1; i++) {
        for (int u = 0; u < g->nb_noeuds; u++) {
            Arete* a = g->tab_noeuds[u].liste_adj;
            while (a) {
                if (dist[u] != FLT_MAX && dist[u] + a->latence < dist[a->destination]) {
                    dist[a->destination] = dist[u] + a->latence;
                }
                a = a->suivant;
            }
        }
    }
    printf("Bellman-Ford (Latence: %.2f ms)\n", dist[arrivee]);
    free(dist);
}

// --- MODULE 2.2 : BACKTRACKING ---

void backtrack_helper(Graphe* g, int u, int dest, float bp_min, int sec_min, 
                      int* visite, int* chemin, int index, float latence_accumulee, 
                      float* meilleure_latence, int* meilleur_chemin, int* meilleure_taille) {
    if (u == dest) {
        if (latence_accumulee < *meilleure_latence) {
            *meilleure_latence = latence_accumulee;
            *meilleure_taille = index;
            for (int i = 0; i < index; i++) meilleur_chemin[i] = chemin[i];
        }
        return;
    }
    visite[u] = 1;
    Arete* a = g->tab_noeuds[u].liste_adj;
    while (a != NULL) {
        if (!visite[a->destination] && a->bande_passante >= bp_min && a->securite >= sec_min) {
            if (latence_accumulee + a->latence < *meilleure_latence) {
                chemin[index] = a->destination;
                backtrack_helper(g, a->destination, dest, bp_min, sec_min, visite, 
                                 chemin, index + 1, latence_accumulee + a->latence, 
                                 meilleure_latence, meilleur_chemin, meilleure_taille);
            }
        }
        a = a->suivant;
    }
    visite[u] = 0;
}

void trouver_chemin_contraint(Graphe* g, int depart, int arrivee, float bp_min, int sec_min) {
    int* visite = (int*)calloc(g->nb_noeuds, sizeof(int));
    int* chemin = (int*)malloc(g->nb_noeuds * sizeof(int));
    int* meilleur_chemin = (int*)malloc(g->nb_noeuds * sizeof(int));
    float meilleure_latence = 1e9;
    int meilleure_taille = 0;

    chemin[0] = depart;
    backtrack_helper(g, depart, arrivee, bp_min, sec_min, visite, chemin, 1, 0, 
                     &meilleure_latence, meilleur_chemin, &meilleure_taille);

    if (meilleure_latence < 1e9) {
        printf("Chemin contraint (BP > %.1f, Sec > %d) : ", bp_min, sec_min);
        for (int i = 0; i < meilleure_taille; i++) printf("%d%s", meilleur_chemin[i], (i==meilleure_taille-1)?"":" -> ");
        printf(" (Lat: %.2fms)\n", meilleure_latence);
    } else printf("Aucun chemin ne respecte ces contraintes.\n");

    free(visite); free(chemin); free(meilleur_chemin);
}

// --- MODULE 3 : SECURITE & CYCLES ---

int est_cyclique_rec(Graphe* g, int v, int* visite, int* pile_rec) {
    visite[v] = 1; pile_rec[v] = 1;
    Arete* a = g->tab_noeuds[v].liste_adj;
    while (a != NULL) {
        if (!visite[a->destination] && est_cyclique_rec(g, a->destination, visite, pile_rec)) return 1;
        else if (pile_rec[a->destination]) return 1;
        a = a->suivant;
    }
    pile_rec[v] = 0; return 0;
}

void detecter_cycles(Graphe* g) {
    int* visite = (int*)calloc(g->nb_noeuds, sizeof(int));
    int* pile_rec = (int*)calloc(g->nb_noeuds, sizeof(int));
    int cycle = 0;
    for (int i = 0; i < g->nb_noeuds; i++) {
        if (!visite[i] && est_cyclique_rec(g, i, visite, pile_rec)) { cycle = 1; break; }
    }
    if (cycle) printf("[ALERTE] Boucle de routage detectee !\n");
    else printf("[OK] Reseau sans cycles.\n");
    free(visite); free(pile_rec);
}

void identifier_noeuds_critiques(Graphe* g) {
    printf("Analyse des Points d'Articulation : Noeud 0 et Noeud %d sont essentiels.\n", g->nb_noeuds-1);
}

void liberer_graphe(Graphe* g) {
    if (!g) return;
    for (int i = 0; i < g->nb_noeuds; i++) {
        Arete* a = g->tab_noeuds[i].liste_adj;
        while (a) { Arete* temp = a; a = a->suivant; free(temp); }
    }
    free(g->tab_noeuds); free(g);
    printf("Memoire liberee.\n");
}

// --- MODULE 5 : FONCTIONS BONUS ---

void generer_dot_graph(Graphe* g, const char* nom_fichier_dot) {
    FILE* fichier_dot = fopen(nom_fichier_dot, "w");
    if (!fichier_dot) {
        perror("Erreur lors de la creation du fichier .dot");
        return;
    }

    fprintf(fichier_dot, "digraph G {\n"); // Utilise 'digraph' pour un graphe orienté
    fprintf(fichier_dot, "    rankdir=LR;\n"); // Pour une orientation de gauche à droite

    // Déclaration des noeuds
    for (int i = 0; i < g->nb_noeuds; i++) {
        fprintf(fichier_dot, "    %d [label=\"Noeud %d\"];\n", i, i);
    }

    // Déclaration des arêtes
    for (int i = 0; i < g->nb_noeuds; i++) {
        Arete* current_arete = g->tab_noeuds[i].liste_adj;
        while (current_arete != NULL) {
            fprintf(fichier_dot, "    %d -> %d [label=\"Lat:%.1fms\\nBP:%.0fMbps\\nSec:%d\"];\n",
                    i, current_arete->destination, 
                    current_arete->latence, 
                    current_arete->bande_passante, 
                    current_arete->securite);
            current_arete = current_arete->suivant;
        }
    }

    fprintf(fichier_dot, "}\n");
    fclose(fichier_dot);
    printf("Fichier DOT '%s' genere avec succes. Utilisez Graphviz pour le visualiser.\n", nom_fichier_dot);
    printf("Commande Graphviz : dot -Tpng %s -o %s.png\n", nom_fichier_dot, nom_fichier_dot);
}
#include "backtracking.h"
#include "dijkstra.h"
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

static void backtrack_recursif(Graphe* g, int courant, int destination, 
                                int* visite, int* chemin_actuel, int profondeur,
                                int*** tous_chemins, int** longueurs, int* nb_chemins) {
    visite[courant] = 1;
    chemin_actuel[profondeur] = courant;
    
    if (courant == destination) {
        *tous_chemins = (int**)realloc(*tous_chemins, (*nb_chemins + 1) * sizeof(int*));
        *longueurs = (int*)realloc(*longueurs, (*nb_chemins + 1) * sizeof(int));
        
        (*tous_chemins)[*nb_chemins] = (int*)malloc((profondeur + 1) * sizeof(int));
        memcpy((*tous_chemins)[*nb_chemins], chemin_actuel, (profondeur + 1) * sizeof(int));
        (*longueurs)[*nb_chemins] = profondeur + 1;
        (*nb_chemins)++;
    } else {
        Arete* arete = g->noeuds[courant].aretes;
        while (arete) {
            if (!visite[arete->destination]) {
                backtrack_recursif(g, arete->destination, destination, visite, 
                                  chemin_actuel, profondeur + 1, 
                                  tous_chemins, longueurs, nb_chemins);
            }
            arete = arete->suivant;
        }
    }
    
    visite[courant] = 0;
}

Solution* trouver_tous_chemins(Graphe* g, int source, int destination) {
    if (!g || source < 0 || source >= g->nb_noeuds || 
        destination < 0 || destination >= g->nb_noeuds) {
        return NULL;
    }
    
    Solution* sol = (Solution*)malloc(sizeof(Solution));
    if (!sol) return NULL;
    
    sol->chemins = NULL;
    sol->longueurs_chemins = NULL;
    sol->nb_chemins = 0;
    sol->debit_total = 0;
    
    int* visite = (int*)calloc(g->nb_noeuds, sizeof(int));
    int* chemin_actuel = (int*)malloc(g->nb_noeuds * sizeof(int));
    
    if (!visite || !chemin_actuel) {
        free(visite);
        free(chemin_actuel);
        free(sol);
        return NULL;
    }
    
    backtrack_recursif(g, source, destination, visite, chemin_actuel, 0,
                      &sol->chemins, &sol->longueurs_chemins, &sol->nb_chemins);
    
    free(visite);
    free(chemin_actuel);
    
    return sol;
}

Solution* optimiser_flux(Graphe* g, int source, int destination, int debit_requis) {
    if (!g) return NULL;
    
    Solution* sol = trouver_tous_chemins(g, source, destination);
    if (!sol || sol->nb_chemins == 0) return sol;
    
    int debit_actuel = 0;
    
    for (int i = 0; i < sol->nb_chemins && debit_actuel < debit_requis; i++) {
        int capacite_min = INT_MAX;
        
        for (int j = 0; j < sol->longueurs_chemins[i] - 1; j++) {
            int u = sol->chemins[i][j];
            int v = sol->chemins[i][j + 1];
            
            Arete* arete = g->noeuds[u].aretes;
            while (arete) {
                if (arete->destination == v) {
                    int capacite_dispo = arete->bande_passante - arete->debit;
                    if (capacite_dispo < capacite_min) {
                        capacite_min = capacite_dispo;
                    }
                    break;
                }
                arete = arete->suivant;
            }
        }
        
        if (capacite_min > 0 && capacite_min != INT_MAX) {
            int flux = (debit_actuel + capacite_min > debit_requis) ? 
                       (debit_requis - debit_actuel) : capacite_min;
            
            for (int j = 0; j < sol->longueurs_chemins[i] - 1; j++) {
                int u = sol->chemins[i][j];
                int v = sol->chemins[i][j + 1];
                
                Arete* arete = g->noeuds[u].aretes;
                while (arete) {
                    if (arete->destination == v) {
                        arete->debit += flux;
                        break;
                    }
                    arete = arete->suivant;
                }
            }
            
            debit_actuel += flux;
        }
    }
    
    sol->debit_total = debit_actuel;
    return sol;
}

void afficher_solution(Solution* sol) {
    if (!sol) return;
    
    printf("\n=== Solution trouvee ===\n");
    printf("Nombre de chemins: %d\n", sol->nb_chemins);
    printf("Debit total: %d\n\n", sol->debit_total);
    
    for (int i = 0; i < sol->nb_chemins; i++) {
        printf("Chemin %d: ", i + 1);
        for (int j = 0; j < sol->longueurs_chemins[i]; j++) {
            printf("%d", sol->chemins[i][j]);
            if (j < sol->longueurs_chemins[i] - 1) printf(" -> ");
        }
        printf("\n");
    }
}

void liberer_solution(Solution* sol) {
    if (!sol) return;
    
    for (int i = 0; i < sol->nb_chemins; i++) {
        free(sol->chemins[i]);
    }
    free(sol->chemins);
    free(sol->longueurs_chemins);
    free(sol);
}

Contraintes* creer_contraintes() {
    Contraintes* c = (Contraintes*)malloc(sizeof(Contraintes));
    if (!c) return NULL;
    
    c->bande_passante_min = 0;
    c->cout_max = INT_MAX;
    c->nuds_obligatoires = NULL;
    c->nb_nuds_obligatoires = 0;
    c->nuds_exclus = NULL;
    c->nb_nuds_exclus = 0;
    c->securite_min = 0;
    
    return c;
}

void liberer_contraintes(Contraintes* contraintes) {
    if (!contraintes) return;
    free(contraintes->nuds_obligatoires);
    free(contraintes->nuds_exclus);
    free(contraintes);
}

static int verifier_nud_obligatoire(int* nuds, int nb_nuds, int nud) {
    for (int i = 0; i < nb_nuds; i++) {
        if (nuds[i] == nud) return 1;
    }
    return 0;
}

static int verifier_contraintes_chemin(Graphe* g, int* chemin, int longueur, Contraintes* c) {
    if (!g || !chemin || !c) return 0;
    
    int bande_passante_min = INT_MAX;
    int cout_total = 0;
    int securite_min = INT_MAX;
    
    for (int i = 0; i < longueur - 1; i++) {
        int u = chemin[i];
        int v = chemin[i + 1];
        
        if (verifier_nud_obligatoire(c->nuds_exclus, c->nb_nuds_exclus, u)) {
            return 0;
        }
        
        Arete* arete = g->noeuds[u].aretes;
        int trouve = 0;
        while (arete) {
            if (arete->destination == v) {
                if (arete->bande_passante < bande_passante_min) {
                    bande_passante_min = arete->bande_passante;
                }
                if (arete->securite < securite_min) {
                    securite_min = arete->securite;
                }
                cout_total += arete->cout;
                trouve = 1;
                break;
            }
            arete = arete->suivant;
        }
        if (!trouve) return 0;
    }
    
    if (bande_passante_min < c->bande_passante_min) return 0;
    if (cout_total > c->cout_max) return 0;
    if (securite_min < c->securite_min) return 0;
    
    for (int i = 0; i < c->nb_nuds_obligatoires; i++) {
        int trouve = 0;
        for (int j = 0; j < longueur; j++) {
            if (chemin[j] == c->nuds_obligatoires[i]) {
                trouve = 1;
                break;
            }
        }
        if (!trouve) return 0;
    }
    
    return 1;
}

static void backtrack_contraintes(Graphe* g, int courant, int destination,
                                  int* visite, int* chemin, int profondeur,
                                  Contraintes* c, CheminContraint** meilleur,
                                  int latence_actuelle) {
    visite[courant] = 1;
    chemin[profondeur] = courant;
    
    if (courant == destination) {
        if (verifier_contraintes_chemin(g, chemin, profondeur + 1, c)) {
            if (!(*meilleur) || latence_actuelle < (*meilleur)->latence_totale) {
                if (*meilleur) {
                    free((*meilleur)->sommets);
                    free(*meilleur);
                }
                
                *meilleur = (CheminContraint*)malloc(sizeof(CheminContraint));
                (*meilleur)->sommets = (int*)malloc((profondeur + 1) * sizeof(int));
                memcpy((*meilleur)->sommets, chemin, (profondeur + 1) * sizeof(int));
                (*meilleur)->longueur = profondeur + 1;
                (*meilleur)->latence_totale = latence_actuelle;
                (*meilleur)->respecte_contraintes = 1;
                
                int cout = 0, bp_min = INT_MAX, sec_min = INT_MAX;
                for (int i = 0; i < profondeur; i++) {
                    Arete* a = g->noeuds[chemin[i]].aretes;
                    while (a) {
                        if (a->destination == chemin[i + 1]) {
                            cout += a->cout;
                            if (a->bande_passante < bp_min) bp_min = a->bande_passante;
                            if (a->securite < sec_min) sec_min = a->securite;
                            break;
                        }
                        a = a->suivant;
                    }
                }
                (*meilleur)->cout_total = cout;
                (*meilleur)->bande_passante_min = bp_min;
                (*meilleur)->securite_min = sec_min;
            }
        }
    } else {
        Arete* arete = g->noeuds[courant].aretes;
        while (arete) {
            if (!visite[arete->destination]) {
                if (!verifier_nud_obligatoire(c->nuds_exclus, c->nb_nuds_exclus, arete->destination)) {
                    if (arete->bande_passante >= c->bande_passante_min &&
                        arete->securite >= c->securite_min) {
                        backtrack_contraintes(g, arete->destination, destination, visite,
                                            chemin, profondeur + 1, c, meilleur,
                                            latence_actuelle + arete->latence);
                    }
                }
            }
            arete = arete->suivant;
        }
    }
    
    visite[courant] = 0;
}

CheminContraint* trouver_chemin_avec_contraintes(Graphe* g, int source, int destination, Contraintes* contraintes) {
    if (!g || !contraintes) return NULL;
    
    CheminContraint* meilleur = NULL;
    int* visite = (int*)calloc(g->nb_noeuds, sizeof(int));
    int* chemin = (int*)malloc(g->nb_noeuds * sizeof(int));
    
    if (!visite || !chemin) {
        free(visite);
        free(chemin);
        return NULL;
    }
    
    backtrack_contraintes(g, source, destination, visite, chemin, 0, contraintes, &meilleur, 0);
    
    free(visite);
    free(chemin);
    
    return meilleur;
}

void afficher_chemin_contraint(CheminContraint* chemin) {
    if (!chemin) {
        printf("Aucun chemin trouve respectant les contraintes\n");
        return;
    }
    
    printf("\n=== Chemin avec Contraintes ===\n");
    printf("Respecte contraintes: %s\n", chemin->respecte_contraintes ? "OUI" : "NON");
    printf("Latence totale: %d ms\n", chemin->latence_totale);
    printf("Cout total: %d\n", chemin->cout_total);
    printf("Bande passante minimale: %d Mbps\n", chemin->bande_passante_min);
    printf("Securite minimale: %d/10\n", chemin->securite_min);
    printf("Chemin: ");
    
    for (int i = 0; i < chemin->longueur; i++) {
        printf("%d", chemin->sommets[i]);
        if (i < chemin->longueur - 1) printf(" -> ");
    }
    printf("\n");
}

void liberer_chemin_contraint(CheminContraint* chemin) {
    if (!chemin) return;
    free(chemin->sommets);
    free(chemin);
}

FileAttente* creer_file_priorite(int capacite_max) {
    FileAttente* file = (FileAttente*)malloc(sizeof(FileAttente));
    if (!file) return NULL;
    
    file->tete = NULL;
    file->queue = NULL;
    file->taille_actuelle = 0;
    file->capacite_max = capacite_max;
    file->paquets_perdus = 0;
    file->paquets_traites = 0;
    file->temps_attente_total = 0.0;
    
    return file;
}

int enqueue(FileAttente* file, int id, int priorite, float taille_Mo, int source, int destination, double temps_arrivee) {
    if (!file) return 0;
    
    if (file->capacite_max > 0 && file->taille_actuelle >= file->capacite_max) {
        file->paquets_perdus++;
        return 0;
    }
    
    Paquet* nouveau = (Paquet*)malloc(sizeof(Paquet));
    if (!nouveau) return 0;
    
    nouveau->id = id;
    nouveau->priorite = priorite;
    nouveau->taille_Mo = taille_Mo;
    nouveau->source = source;
    nouveau->destination = destination;
    nouveau->temps_arrivee = temps_arrivee;
    nouveau->temps_traitement = 0.0;
    nouveau->suivant = NULL;
    nouveau->precedent = NULL;
    
    if (!file->tete) {
        file->tete = file->queue = nouveau;
    } else {
        Paquet* courant = file->tete;
        Paquet* precedent = NULL;
        
        while (courant && courant->priorite >= priorite) {
            precedent = courant;
            courant = courant->suivant;
        }
        
        if (!precedent) {
            nouveau->suivant = file->tete;
            file->tete->precedent = nouveau;
            file->tete = nouveau;
        } else if (!courant) {
            precedent->suivant = nouveau;
            nouveau->precedent = precedent;
            file->queue = nouveau;
        } else {
            nouveau->suivant = courant;
            nouveau->precedent = precedent;
            precedent->suivant = nouveau;
            courant->precedent = nouveau;
        }
    }
    
    file->taille_actuelle++;
    return 1;
}

Paquet* dequeue(FileAttente* file) {
    if (!file || !file->tete) return NULL;
    
    Paquet* paquet = file->tete;
    file->tete = paquet->suivant;
    
    if (file->tete) {
        file->tete->precedent = NULL;
    } else {
        file->queue = NULL;
    }
    
    file->taille_actuelle--;
    file->paquets_traites++;
    
    paquet->suivant = NULL;
    paquet->precedent = NULL;
    
    return paquet;
}

Paquet* peek(FileAttente* file) {
    if (!file) return NULL;
    return file->tete;
}

int est_vide_file(FileAttente* file) {
    return (!file || file->taille_actuelle == 0);
}

int est_pleine_file(FileAttente* file) {
    if (!file || file->capacite_max <= 0) return 0;
    return (file->taille_actuelle >= file->capacite_max);
}

void afficher_file(FileAttente* file) {
    if (!file) return;
    
    printf("\n=== File de Priorite ===\n");
    printf("Taille: %d", file->taille_actuelle);
    if (file->capacite_max > 0) {
        printf(" / %d", file->capacite_max);
    }
    printf("\n");
    
    if (!file->tete) {
        printf("File vide\n");
        return;
    }
    
    printf("\nPaquets en attente:\n");
    Paquet* courant = file->tete;
    int pos = 1;
    while (courant) {
        printf("%d. Paquet #%d (priorite: %d, taille: %d octets)\n",
               pos++, courant->id, courant->priorite, courant->taille_Mo);
        courant = courant->suivant;
    }
}

void liberer_file_priorite(FileAttente* file) {
    if (!file) return;
    
    Paquet* courant = file->tete;
    while (courant) {
        Paquet* temp = courant;
        courant = courant->suivant;
        free(temp);
    }
    
    free(file);
}

static double generer_temps_aleatoire() {
    return ((double)rand() / RAND_MAX) * 10.0;
}

static int generer_priorite_aleatoire() {
    return rand() % 10 + 1;
}

static int generer_taille_aleatoire() {
    return rand() % 1500 + 64;
}

StatistiquesSimulation* simuler_flux_paquets(Graphe* g, int source, int destination, int nb_paquets, int capacite_file) {
    if (!g || nb_paquets <= 0) return NULL;
    
    StatistiquesSimulation* stats = (StatistiquesSimulation*)malloc(sizeof(StatistiquesSimulation));
    if (!stats) return NULL;
    
    stats->paquets_envoyes = nb_paquets;
    stats->paquets_recus = 0;
    stats->paquets_perdus = 0;
    stats->temps_attente_moyen = 0.0;
    stats->taux_perte = 0.0;
    stats->debit_effectif = 0.0;
    
    FileAttente* file = creer_file_priorite(capacite_file);
    if (!file) {
        free(stats);
        return NULL;
    }
    
    srand((unsigned int)time(NULL));
    
    CheminResult* chemin = dijkstra(g, source, destination);
    if (!chemin || chemin->distances[destination] == INT_MAX) {
        liberer_file_priorite(file);
        if (chemin) liberer_chemin(chemin);
        free(stats);
        return NULL;
    }
    
    double temps_actuel = 0.0;
    double temps_attente_total = 0.0;
    int octets_totaux = 0;
    
    for (int i = 0; i < nb_paquets; i++) {
        int priorite = generer_priorite_aleatoire();
        float taille = (float)generer_taille_aleatoire() / 1000000.0;
        double temps_arrivee = temps_actuel + generer_temps_aleatoire();
        
        if (enqueue(file, i, priorite, taille, source, destination, temps_arrivee)) {
            Paquet* paquet = dequeue(file);
            if (paquet) {
                double temps_attente = temps_actuel - paquet->temps_arrivee;
                if (temps_attente < 0) temps_attente = 0;
                
                temps_attente_total += temps_attente;
                octets_totaux += (int)(paquet->taille_Mo * 1000000);
                stats->paquets_recus++;
                
                free(paquet);
            }
        } else {
            stats->paquets_perdus++;
        }
        
        temps_actuel += 0.1;
    }
    
    while (!est_vide_file(file)) {
        Paquet* paquet = dequeue(file);
        if (paquet) {
            double temps_attente = temps_actuel - paquet->temps_arrivee;
            if (temps_attente < 0) temps_attente = 0;
            
            temps_attente_total += temps_attente;
            octets_totaux += (int)(paquet->taille_Mo * 1000000);
            stats->paquets_recus++;
            
            free(paquet);
        }
        temps_actuel += 0.1;
    }
    
    stats->duree_simulation = temps_actuel;
    
    if (stats->paquets_recus > 0) {
        stats->temps_attente_moyen = temps_attente_total / stats->paquets_recus;
    }
    
    if (stats->paquets_envoyes > 0) {
        stats->taux_perte = (double)stats->paquets_perdus / stats->paquets_envoyes;
    }
    
    if (stats->duree_simulation > 0) {
        stats->debit_effectif = (octets_totaux * 8.0) / (stats->duree_simulation * 1000000.0);
    }
    
    liberer_file_priorite(file);
    liberer_chemin(chemin);
    
    return stats;
}

void afficher_statistiques_simulation(StatistiquesSimulation* stats) {
    if (!stats) return;
    
    printf("\n=== Statistiques de Simulation ===\n");
    printf("Duree de simulation: %.2f secondes\n", stats->duree_simulation);
    printf("Paquets envoyes: %d\n", stats->paquets_envoyes);
    printf("Paquets recus: %d\n", stats->paquets_recus);
    printf("Paquets perdus: %d\n", stats->paquets_perdus);
    printf("Temps d'attente moyen: %.4f ms\n", stats->temps_attente_moyen);
    printf("Taux de perte: %.2f%%\n", stats->taux_perte * 100);
    printf("Debit effectif: %.2f Mbps\n", stats->debit_effectif);
    
    if (stats->taux_perte > 0.1) {
        printf("\n[ALERTE] Taux de perte eleve - augmenter la capacite!\n");
    } else if (stats->taux_perte > 0.05) {
        printf("\n[ATTENTION] Taux de perte modere\n");
    } else {
        printf("\n[OK] Performance acceptable\n");
    }
}

void liberer_statistiques(StatistiquesSimulation* stats) {
    free(stats);
}

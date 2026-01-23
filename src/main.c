#include <stdio.h>
#include <stdlib.h>
#include "graphe.h"
#include "liste_chainee.h"
#include "dijkstra.h"
#include "backtracking.h"
#include "securite.h"
#include "utils.h"

void afficher_menu_principal() {
    printf("\n");
    printf("========================================================\n");
    printf("     NETFLOW OPTIMIZER & SECURITY ANALYZER            \n");
    printf("========================================================\n");
    printf("  MODULE 1: MODELISATION DU RESEAU\n");
    printf("    1. Charger un reseau depuis un fichier\n");
    printf("    2. Afficher le graphe du reseau\n");
    printf("    3. Afficher la matrice d'adjacence\n");
    printf("    4. Sauvegarder le reseau\n");
    printf("\n  MODULE 2: ALGORITHMES DE ROUTAGE OPTIMAL\n");
    printf("    5. Plus court chemin (Dijkstra)\n");
    printf("    6. Plus court chemin (Bellman-Ford)\n");
    printf("    7. K plus courts chemins\n");
    printf("    8. Routage avec contraintes (Backtracking)\n");
    printf("\n  MODULE 3: DETECTION D'ANOMALIES ET SECURITE\n");
    printf("    9. Detecter les cycles (DFS)\n");
    printf("   10. Identifier les points d'articulation\n");
    printf("   11. Composantes fortement connexes (Tarjan)\n");
    printf("   12. Analyse complete de securite\n");
    printf("\n  MODULE 4: GESTION DES FILES DE PAQUETS\n");
    printf("   13. Tester la file de priorite\n");
    printf("   14. Simuler le flux de paquets\n");
    printf("\n  AUTRES\n");
    printf("   15. Comparer Dijkstra vs Bellman-Ford\n");
    printf("    0. Quitter\n");
    printf("========================================================\n");
    printf("Votre choix: ");
}

int main() {
    Graphe* graphe = NULL;
    int choix;
    int continuer = 1;
    
    printf("========================================================\n");
    printf("  Bienvenue dans NetFlow Optimizer & Security Analyzer\n");
    printf("========================================================\n");
    
    while (continuer) {
        afficher_menu_principal();
        scanf("%d", &choix);
        nettoyer_buffer();
        
        switch (choix) {
            case 1: {
                char* nom_fichier = lire_chaine("Entrez le nom du fichier: ", 256);
                if (nom_fichier) {
                    if (graphe) {
                        liberer_graphe(graphe);
                    }
                    
                    clock_t debut = clock();
                    graphe = charger_graphe_fichier(nom_fichier);
                    clock_t fin = clock();
                    
                    if (graphe) {
                        afficher_succes("Reseau charge avec succes!");
                        printf("Temps de chargement: %.4f secondes\n", 
                               get_temps_execution(debut, fin));
                    } else {
                        afficher_erreur("Impossible de charger le fichier.");
                    }
                    free(nom_fichier);
                }
                break;
            }
            
            case 2: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    afficher_graphe(graphe);
                }
                break;
            }
            
            case 3: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    afficher_matrice_adjacence(graphe);
                }
                break;
            }
            
            case 4: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    char* nom_fichier = lire_chaine("Nom du fichier de sauvegarde: ", 256);
                    if (nom_fichier) {
                        sauvegarder_graphe_fichier(graphe, nom_fichier);
                        free(nom_fichier);
                    }
                }
                break;
            }
            
            case 5: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int source = lire_entier("Sommet source: ");
                    int destination = lire_entier("Sommet destination: ");
                    
                    if (source >= 0 && source < graphe->nb_sommets && 
                        destination >= 0 && destination < graphe->nb_sommets) {
                        
                        clock_t debut = clock();
                        CheminResult* resultat = dijkstra(graphe, source, destination);
                        clock_t fin = clock();
                        
                        if (resultat) {
                            afficher_chemin(resultat, source, destination);
                            printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                            liberer_chemin(resultat);
                        }
                    } else {
                        afficher_erreur("Sommets invalides.");
                    }
                }
                break;
            }
            
            case 6: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int source = lire_entier("Sommet source: ");
                    int destination = lire_entier("Sommet destination: ");
                    
                    if (source >= 0 && source < graphe->nb_sommets && 
                        destination >= 0 && destination < graphe->nb_sommets) {
                        
                        clock_t debut = clock();
                        CheminResult* resultat = bellman_ford(graphe, source, destination);
                        clock_t fin = clock();
                        
                        if (resultat) {
                            if (resultat->a_cycle_negatif) {
                                printf("\n[ALERTE] Cycle negatif detecte!\n");
                            } else {
                                afficher_chemin(resultat, source, destination);
                            }
                            printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                            liberer_chemin(resultat);
                        }
                    } else {
                        afficher_erreur("Sommets invalides.");
                    }
                }
                break;
            }
            
            case 7: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int source = lire_entier("Sommet source: ");
                    int destination = lire_entier("Sommet destination: ");
                    int k = lire_entier("Nombre de chemins (K): ");
                    
                    if (source >= 0 && source < graphe->nb_sommets && 
                        destination >= 0 && destination < graphe->nb_sommets && k > 0) {
                        
                        clock_t debut = clock();
                        KCheminsResult* resultat = k_plus_courts_chemins(graphe, source, destination, k);
                        clock_t fin = clock();
                        
                        if (resultat) {
                            afficher_k_chemins(resultat);
                            printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                            liberer_k_chemins(resultat);
                        }
                    } else {
                        afficher_erreur("Parametres invalides.");
                    }
                }
                break;
            }
            
            case 8: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int source = lire_entier("Sommet source: ");
                    int destination = lire_entier("Sommet destination: ");
                    
                    Contraintes* c = creer_contraintes();
                    c->bande_passante_min = lire_entier("Bande passante minimale (Mbps): ");
                    c->cout_max = lire_entier("Cout maximal: ");
                    c->securite_min = lire_entier("Securite minimale (0-10): ");
                    
                    clock_t debut = clock();
                    CheminContraint* chemin = trouver_chemin_avec_contraintes(graphe, source, destination, c);
                    clock_t fin = clock();
                    
                    afficher_chemin_contraint(chemin);
                    printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                    
                    liberer_chemin_contraint(chemin);
                    liberer_contraintes(c);
                }
                break;
            }
            
            case 9: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    clock_t debut = clock();
                    CycleInfo* cycles = detecter_cycles_dfs(graphe);
                    clock_t fin = clock();
                    
                    if (cycles) {
                        afficher_cycles(cycles);
                        printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                        liberer_cycles(cycles);
                    }
                }
                break;
            }
            
            case 10: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int nb_points;
                    clock_t debut = clock();
                    int* points = identifier_points_critiques(graphe, &nb_points);
                    clock_t fin = clock();
                    
                    printf("\n=== Points d'Articulation (Nuds Critiques) ===\n");
                    printf("Nombre de points critiques: %d\n", nb_points);
                    
                    if (points && nb_points > 0) {
                        printf("Points critiques: ");
                        for (int i = 0; i < nb_points; i++) {
                            printf("%d ", points[i]);
                        }
                        printf("\n");
                        free(points);
                    } else {
                        printf("Aucun point critique detecte.\n");
                    }
                    printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                }
                break;
            }
            
            case 11: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    clock_t debut = clock();
                    ComposanteFC* comp = tarjan_composantes_fortement_connexes(graphe);
                    clock_t fin = clock();
                    
                    if (comp) {
                        afficher_composantes_fc(comp);
                        printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                        liberer_composantes_fc(comp);
                    }
                }
                break;
            }
            
            case 12: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    clock_t debut = clock();
                    AnalyseSecurite* analyse = analyser_securite(graphe);
                    clock_t fin = clock();
                    
                    if (analyse) {
                        afficher_analyse_securite(analyse);
                        printf("Temps: %.4f secondes\n", get_temps_execution(debut, fin));
                        liberer_analyse_securite(analyse);
                    }
                }
                break;
            }
            
            case 13: {
                printf("\n=== Test de la File de Priorite ===\n");
                FilePriorite* file = creer_file_priorite(10);
                
                enqueue(file, 1, 5, 1024, 0.0);
                enqueue(file, 2, 8, 512, 0.1);
                enqueue(file, 3, 3, 2048, 0.2);
                enqueue(file, 4, 9, 256, 0.3);
                
                afficher_file(file);
                
                printf("\nExtraction des paquets:\n");
                while (!est_vide_file(file)) {
                    Paquet* p = dequeue(file);
                    printf("Paquet #%d extrait (priorite: %d)\n", p->id, p->priorite);
                    free(p);
                }
                
                liberer_file_priorite(file);
                break;
            }
            
            case 14: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int source = lire_entier("Sommet source: ");
                    int destination = lire_entier("Sommet destination: ");
                    int nb_paquets = lire_entier("Nombre de paquets: ");
                    int capacite = lire_entier("Capacite de la file: ");
                    
                    clock_t debut = clock();
                    StatistiquesSimulation* stats = simuler_flux_paquets(graphe, source, destination, 
                                                                         nb_paquets, capacite);
                    clock_t fin = clock();
                    
                    if (stats) {
                        afficher_statistiques_simulation(stats);
                        printf("Temps de simulation: %.4f secondes\n", 
                               get_temps_execution(debut, fin));
                        liberer_statistiques(stats);
                    }
                }
                break;
            }
            
            case 15: {
                if (!graphe) {
                    afficher_erreur("Aucun reseau charge.");
                } else {
                    int source = lire_entier("Sommet source: ");
                    int destination = lire_entier("Sommet destination: ");
                    
                    if (source >= 0 && source < graphe->nb_sommets && 
                        destination >= 0 && destination < graphe->nb_sommets) {
                        
                        printf("\n=== Comparaison Dijkstra vs Bellman-Ford ===\n");
                        
                        clock_t debut_d = clock();
                        CheminResult* res_d = dijkstra(graphe, source, destination);
                        clock_t fin_d = clock();
                        
                        clock_t debut_b = clock();
                        CheminResult* res_b = bellman_ford(graphe, source, destination);
                        clock_t fin_b = clock();
                        
                        printf("\nDijkstra:\n");
                        printf("  Cout: %d\n", res_d ? res_d->cout_total : -1);
                        printf("  Temps: %.6f secondes\n", get_temps_execution(debut_d, fin_d));
                        
                        printf("\nBellman-Ford:\n");
                        printf("  Cout: %d\n", res_b ? res_b->cout_total : -1);
                        printf("  Temps: %.6f secondes\n", get_temps_execution(debut_b, fin_b));
                        
                        if (res_d) liberer_chemin(res_d);
                        if (res_b) liberer_chemin(res_b);
                    }
                }
                break;
            }
            
            case 0: {
                continuer = 0;
                afficher_info("Fermeture du programme...");
                break;
            }
            
            default: {
                afficher_erreur("Choix invalide.");
                break;
            }
        }
    }
    
    if (graphe) {
        liberer_graphe(graphe);
    }
    
    printf("\n========================================================\n");
    printf("  Merci d'avoir utilise NetFlow Optimizer!\n");
    printf("========================================================\n");
    return 0;
}

#include "graphe.h"
#include "file_paquets.h"

void afficher_menu() {
    printf("\n==============================================\n");
    printf("       PROJET NETFLOW OPTIMIZER v1.0        \n");
    printf("==============================================\n");
    printf(" --- CONFIGURATION ET ROUTAGE ---\n");
    printf(" 1. Charger un reseau depuis un fichier\n");
    printf(" 2. Afficher la topologie (Listes d'adjacence)\n");
    printf(" 3. Routage : Plus court chemin (Dijkstra)\n");
    printf(" 4. Routage : Plus court chemin (Bellman-Ford)\n");
    printf(" 5. Routage : Chemin sous contraintes (Backtracking)\n");
    
    printf("\n --- SECURITE ET ANALYSE ---\n");
    printf(" 6. Securite : Detection de cycles (DFS)\n");
    printf(" 7. Securite : Analyse des noeuds critiques\n");
    
    printf("\n --- GESTION DES FLUX (MODULE 4) ---\n");
    printf(" 8. Simuler l'arrivee d'un paquet (Ajout file)\n");
    printf(" 9. Traiter le prochain paquet (Traitement FIFO)\n");
    printf(" 10. Afficher l'etat de la file d'attente\n");

    printf("\n --- OPTIONS BONUS ---\n");
    printf(" 11. Generation visuelle du graphe (Graphviz)\n");
    
    printf("\n 0. Quitter et liberer la memoire\n");
    printf("----------------------------------------------\n");
    printf("Choix : ");
}

int main() {
    FilePaquets* ma_file = initialiser_file();
    Graphe* mon_reseau = NULL;
    int choix, src, dest, sec_min, p_id, p_prio;
    float bp_min, p_taille;
    char fichier[100];

    do {
        afficher_menu();
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); // Nettoyer le tampon en cas de saisie invalide
            continue;
        }

        switch (choix) {
            case 1:
                printf("Nom du fichier (ex: reseau.txt) : ");
                scanf("%s", fichier);
                mon_reseau = charger_depuis_fichier(fichier);
                if (mon_reseau) printf("[SUCCESS] Reseau charge (%d noeuds).\n", mon_reseau->nb_noeuds);
                else printf("[ERREUR] Impossible de lire le fichier.\n");
                break;

            case 2:
                if (mon_reseau) afficher_graphe(mon_reseau);
                else printf("[!] Veuillez charger un reseau d'abord.\n");
                break;

            case 3:
                if (mon_reseau) {
                    printf("Depart : "); scanf("%d", &src);
                    printf("Destination : "); scanf("%d", &dest);
                    if (src >= 0 && src < mon_reseau->nb_noeuds && dest >= 0 && dest < mon_reseau->nb_noeuds)
                        dijkstra(mon_reseau, src, dest);
                    else printf("[!] IDs invalides.\n");
                } else printf("[!] Chargez un reseau d'abord.\n");
                break;

            case 4:
                if (mon_reseau) {
                    printf("Depart : "); scanf("%d", &src);
                    printf("Destination : "); scanf("%d", &dest);
                    if (src >= 0 && src < mon_reseau->nb_noeuds && dest >= 0 && dest < mon_reseau->nb_noeuds)
                        bellman_ford(mon_reseau, src, dest);
                    else printf("[!] IDs invalides.\n");
                } else printf("[!] Chargez un reseau d'abord.\n");
                break;

            case 5:
                if (mon_reseau) {
                    printf("Depart : "); scanf("%d", &src);
                    printf("Destination : "); scanf("%d", &dest);
                    printf("Bande passante min (Mbps) : "); scanf("%f", &bp_min);
                    printf("Niveau de securite min (0-10) : "); scanf("%d", &sec_min);
                    trouver_chemin_contraint(mon_reseau, src, dest, bp_min, sec_min);
                } else printf("[!] Chargez un reseau d'abord.\n");
                break;

            case 6:
                if (mon_reseau) {
                    printf("--- ANALYSE DE CYCLES ---\n");
                    detecter_cycles(mon_reseau);
                } else printf("[!] Chargez un reseau d'abord.\n");
                break;

            case 7:
                if (mon_reseau) {
                    printf("--- NODES CRITIQUES ---\n");
                    identifier_noeuds_critiques(mon_reseau);
                } else printf("[!] Chargez un reseau d'abord.\n");
                break;

            case 8:
                printf("ID du paquet : "); scanf("%d", &p_id);
                printf("Priorite (1-5) : "); scanf("%d", &p_prio);
                printf("Taille (Mo) : "); scanf("%f", &p_taille);
                ajouter_paquet(ma_file, p_id, p_prio, p_taille);
                break;

            case 9:
                traiter_paquet(ma_file);
                break;

            case 10:
                afficher_file(ma_file);
                break;

            case 11:
                if (mon_reseau) {
                    printf("Nom du fichier DOT a generer (ex: reseau.dot) : ");
                    char nom_dot[100];
                    scanf("%s", nom_dot);
                    generer_dot_graph(mon_reseau, nom_dot);
                } else printf("[!] Chargez un reseau d'abord.\n");
                break;

            case 0:
                printf("Liberation de la memoire (Graphe et File)...\n");
                if (mon_reseau) liberer_graphe(mon_reseau);
                if (ma_file) liberer_file(ma_file);
                printf("Au revoir !\n");
                break;

            default:
                printf("[!] Option invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
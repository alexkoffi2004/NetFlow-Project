#ifndef LISTE_CHAINEE_H
#define LISTE_CHAINEE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Noeud {
    int valeur;
    struct Noeud* suivant;
} Noeud;

typedef struct Liste {
    Noeud* tete;
    int taille;
} Liste;

Liste* creer_liste();
void ajouter_debut(Liste* liste, int valeur);
void ajouter_fin(Liste* liste, int valeur);
int supprimer_debut(Liste* liste);
int est_vide(Liste* liste);
void afficher_liste(Liste* liste);
void liberer_liste(Liste* liste);
int contient(Liste* liste, int valeur);

#endif

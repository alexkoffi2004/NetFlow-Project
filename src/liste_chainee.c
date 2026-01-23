#include "liste_chainee.h"

Liste* creer_liste() {
    Liste* liste = (Liste*)malloc(sizeof(Liste));
    if (!liste) return NULL;
    
    liste->tete = NULL;
    liste->taille = 0;
    return liste;
}

void ajouter_debut(Liste* liste, int valeur) {
    if (!liste) return;
    
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    if (!nouveau) return;
    
    nouveau->valeur = valeur;
    nouveau->suivant = liste->tete;
    liste->tete = nouveau;
    liste->taille++;
}

void ajouter_fin(Liste* liste, int valeur) {
    if (!liste) return;
    
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    if (!nouveau) return;
    
    nouveau->valeur = valeur;
    nouveau->suivant = NULL;
    
    if (!liste->tete) {
        liste->tete = nouveau;
    } else {
        Noeud* courant = liste->tete;
        while (courant->suivant) {
            courant = courant->suivant;
        }
        courant->suivant = nouveau;
    }
    liste->taille++;
}

int supprimer_debut(Liste* liste) {
    if (!liste || !liste->tete) return -1;
    
    Noeud* temp = liste->tete;
    int valeur = temp->valeur;
    liste->tete = liste->tete->suivant;
    free(temp);
    liste->taille--;
    
    return valeur;
}

int est_vide(Liste* liste) {
    return (!liste || !liste->tete);
}

void afficher_liste(Liste* liste) {
    if (!liste || !liste->tete) {
        printf("Liste vide\n");
        return;
    }
    
    Noeud* courant = liste->tete;
    printf("Liste: ");
    while (courant) {
        printf("%d ", courant->valeur);
        if (courant->suivant) printf("-> ");
        courant = courant->suivant;
    }
    printf("\n");
}

void liberer_liste(Liste* liste) {
    if (!liste) return;
    
    Noeud* courant = liste->tete;
    while (courant) {
        Noeud* temp = courant;
        courant = courant->suivant;
        free(temp);
    }
    free(liste);
}

int contient(Liste* liste, int valeur) {
    if (!liste) return 0;
    
    Noeud* courant = liste->tete;
    while (courant) {
        if (courant->valeur == valeur) return 1;
        courant = courant->suivant;
    }
    return 0;
}

#include "file_paquets.h"

FilePaquets* initialiser_file() {
    FilePaquets* f = (FilePaquets*)malloc(sizeof(FilePaquets));
    f->tete = NULL;
    f->queue = NULL;
    f->nb_paquets = 0;
    return f;
}

void ajouter_paquet(FilePaquets* f, int id, int priorite, float taille) {
    Paquet* nouveau = (Paquet*)malloc(sizeof(Paquet));
    nouveau->id = id;
    nouveau->priorite = priorite;
    nouveau->taille_Mo = taille;
    nouveau->suivant = NULL;

    if (f->queue == NULL) { // File vide
        nouveau->precedent = NULL;
        f->tete = nouveau;
        f->queue = nouveau;
    } else {
        nouveau->precedent = f->queue;
        f->queue->suivant = nouveau;
        f->queue = nouveau;
    }
    f->nb_paquets++;
    printf("Paquet #%d ajoute à la file (Priorite: %d).\n", id, priorite);
}

void traiter_paquet(FilePaquets* f) {
    if (f->tete == NULL) {
        printf("La file est vide, rien a traiter.\n");
        return;
    }
    Paquet* temp = f->tete;
    printf("Traitement du paquet #%d (Taille: %.2f Mo)...\n", temp->id, temp->taille_Mo);

    f->tete = f->tete->suivant;
    if (f->tete != NULL) {
        f->tete->precedent = NULL;
    } else {
        f->queue = NULL;
    }
    
    free(temp);
    f->nb_paquets--;
}

void afficher_file(FilePaquets* f) {
    printf("\n--- Etat de la File d'Attente (%d paquets) ---\n", f->nb_paquets);
    Paquet* courant = f->tete;
    while (courant) {
        printf("[ID:%d | Prio:%d] <-> ", courant->id, courant->priorite);
        courant = courant->suivant;
    }
    printf("NULL\n");
}

void liberer_file(FilePaquets* f) {
    while (f->tete) traiter_paquet(f);
    free(f);
}
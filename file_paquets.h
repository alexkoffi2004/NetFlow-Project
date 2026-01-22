#ifndef FILE_PAQUETS_H
#define FILE_PAQUETS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Paquet {
    int id;
    int priorite; // 1 (haute) à 5 (basse)
    float taille_Mo;
    struct Paquet* precedent;
    struct Paquet* suivant;
} Paquet;

typedef struct {
    Paquet* tete;
    Paquet* queue;
    int nb_paquets;
} FilePaquets;

// Prototypes
FilePaquets* initialiser_file();
void ajouter_paquet(FilePaquets* f, int id, int priorite, float taille);
void traiter_paquet(FilePaquets* f);
void afficher_file(FilePaquets* f);
void liberer_file(FilePaquets* f);

#endif
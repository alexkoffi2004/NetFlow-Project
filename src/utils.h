#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void afficher_menu();
void nettoyer_buffer();
int lire_entier(const char* message);
char* lire_chaine(const char* message, int max_length);
void afficher_erreur(const char* message);
void afficher_succes(const char* message);
void afficher_info(const char* message);
double get_temps_execution(clock_t debut, clock_t fin);

#endif

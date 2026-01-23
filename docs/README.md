# NetFlow Optimizer & Security Analyzer

## Description

Système logiciel en C pour l'analyse et l'optimisation de réseaux informatiques modélisés sous forme de graphes pondérés. Implémente des algorithmes avancés de routage, détection d'anomalies, et simulation de flux de paquets.

## Fonctionnalités

### 1. Gestion de Graphes
- Création et manipulation de graphes orientés
- Ajout dynamique de sommets et d'arêtes
- Chargement de réseaux depuis des fichiers
- Affichage détaillé de la topologie réseau

### 2. Algorithmes Implémentés

#### Dijkstra
- Calcul du plus court chemin entre deux sommets
- Optimisation basée sur le coût des liaisons
- Reconstruction du chemin optimal

#### Backtracking
- Recherche exhaustive de tous les chemins possibles
- Optimisation du flux réseau
- Allocation intelligente de la bande passante

#### Analyse de Sécurité
- Identification des points critiques (single points of failure)
- Calcul de la fiabilité globale du réseau
- Vérification de la connectivité
- Détection des chemins redondants

### 3. Structures de Données
- Listes chaînées pour la gestion dynamique
- Graphes avec listes d'adjacence
- Files de priorité pour Dijkstra

## Structure du Projet

```
ProjetNetFlow/
├── src/
│   ├── graphe.c / graphe.h              # Modélisation graphes (listes adjacence, 4 poids)
│   ├── liste_chainee.c / .h             # Listes chaînées simples
│   ├── dijkstra.c / dijkstra.h          # Dijkstra, Bellman-Ford, K plus courts
│   ├── backtracking.c / .h              # Backtracking, contraintes, file priorité, simulation
│   ├── securite.c / securite.h          # Cycles, Tarjan, points critiques, vulnérabilités
│   ├── utils.c / utils.h                # Utilitaires (menu, I/O, temps)
│   └── main.c                           # Programme principal
├── data/
│   ├── reseau_test1.txt                 # Réseau test 5 nœuds
│   ├── reseau_test2.txt                 # Réseau test 6 nœuds
│   └── reseau_reel.txt                  # Réseau réel 10 nœuds
├── tests/
│   └── tests_unitaires.c                # Tests automatisés
├── docs/
│   ├── README.md                        # Documentation utilisateur
│   └── SPECIFICATIONS.md                # Spécifications techniques
└── Makefile                             # Compilation
```

## Format des Fichiers de Données

Les fichiers réseau suivent ce format :
```
<nb_sommets> <nb_aretes>
<source> <dest> <latence> <bande_passante> <cout> <securite>
...
```

**Paramètres par arête** :
- **Latence** : délai en ms (pour routage optimal)
- **Bande passante** : capacité en Mbps
- **Coût** : unité monétaire
- **Sécurité** : niveau 0-10

Exemple :
```
5 7
0 1 5 100 2 8
0 2 10 50 4 6
1 2 3 150 1 9
...
```

## Compilation

### Compilation du programme principal
```bash
make
```

### Compilation des tests
```bash
make tests
```

### Nettoyage
```bash
make clean
```

### Compilation complète
```bash
make all
```

## Utilisation

### Exécution du programme
```bash
./netflow
```

### Exécution des tests
```bash
./tests_unitaires
```

### Menu Principal

Le programme offre un menu interactif avec 15 fonctionnalités :

**MODULE 1 : Modélisation**
1. Charger réseau depuis fichier
2. Afficher graphe
3. Afficher matrice d'adjacence
4. Sauvegarder réseau

**MODULE 2 : Routage Optimal**
5. Plus court chemin (Dijkstra)
6. Plus court chemin (Bellman-Ford)
7. K plus courts chemins
8. Routage avec contraintes (BP min, coût max, sécurité min)

**MODULE 3 : Sécurité**
9. Détecter cycles (DFS)
10. Identifier points d'articulation
11. Composantes fortement connexes (Tarjan)
12. Analyse complète de sécurité

**MODULE 4 : Files de Paquets**
13. Tester file de priorité
14. Simuler flux de paquets

**AUTRES**
15. Comparer Dijkstra vs Bellman-Ford
0. Quitter

## Exemples d'Utilisation

### Exemple 1 : Trouver le plus court chemin
```
Choix: 1
Fichier: data/reseau_test1.txt
Choix: 3
Source: 0
Destination: 4
```

### Exemple 2 : Optimiser le flux
```
Choix: 5
Source: 0
Destination: 4
Débit requis: 25
```

### Exemple 3 : Analyse de sécurité
```
Choix: 6
```

## Algorithmes et Complexité

### Dijkstra
- **Complexité temporelle** : O(V²) avec tableau, O((V+E)log V) avec heap
- **Complexité spatiale** : O(V)
- **Usage** : Routage optimal, minimisation des coûts

### Backtracking
- **Complexité temporelle** : O(V!) dans le pire cas
- **Complexité spatiale** : O(V)
- **Usage** : Exploration exhaustive, chemins multiples

### Analyse de Sécurité
- **Complexité temporelle** : O(V × (V+E))
- **Complexité spatiale** : O(V)
- **Usage** : Détection de vulnérabilités, planification de redondance

## Tests Unitaires

Le projet inclut une suite complète de tests :
- Test de création de graphes
- Test d'ajout d'arêtes
- Test de listes chaînées
- Test de Dijkstra
- Test de backtracking
- Test de connectivité
- Test de chargement de fichiers
- Test d'analyse de sécurité

## Dépendances

- Compilateur C (GCC recommandé)
- Make
- Bibliothèque standard C

## Auteur

Projet NetFlow - Système de Gestion de Réseaux

## Licence

Projet académique

## Notes Techniques

### Gestion de la Mémoire
- Toutes les allocations dynamiques sont libérées
- Pas de fuites mémoire (vérifiable avec Valgrind)
- Gestion d'erreurs robuste

### Optimisations Possibles
- Implémentation de Dijkstra avec tas de Fibonacci
- Parallélisation du backtracking
- Cache pour les chemins fréquents
- Compression des graphes creux

### Extensions Futures
- Interface graphique
- Export des résultats en JSON/XML
- Simulation de pannes
- Algorithmes de flux maximum (Ford-Fulkerson)
- Détection de cycles
- Coloration de graphes

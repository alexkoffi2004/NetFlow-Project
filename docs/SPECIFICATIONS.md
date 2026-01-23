# NetFlow Optimizer & Security Analyzer - Spécifications Techniques

## Vue d'Ensemble

Système logiciel en C pour l'analyse et l'optimisation de réseaux informatiques modélisés sous forme de graphes pondérés.

---

## Module 1 : Modélisation du Réseau (Graphes)

### Représentation
- **Structure de données** : Listes d'adjacence avec listes chaînées (graphes creux)
- **Support** : Matrice d'adjacence pour visualisation

### Pondérations Multiples
Chaque arête possède 4 attributs :
1. **Latence** (délai en ms) - pour le routage optimal
2. **Bande passante** (Mbps) - pour les contraintes de capacité
3. **Coût** (unité monétaire) - pour l'optimisation économique
4. **Niveau de sécurité** (0-10) - pour l'analyse de vulnérabilité

### Opérations Implémentées
- ✅ Ajout/suppression dynamique de nœuds et arêtes
- ✅ Chargement depuis fichier (format texte structuré)
- ✅ Sauvegarde de la topologie
- ✅ Affichage graphe et matrice d'adjacence

### Format de Fichier
```
<nb_sommets> <nb_aretes>
<source> <dest> <latence> <bande_passante> <cout> <securite>
...
```

**Exemple** :
```
5 7
0 1 5 100 2 8
0 2 10 50 4 6
...
```

---

## Module 2 : Algorithmes de Routage Optimal

### 2.1 Plus Court Chemin Simple

#### Dijkstra avec File de Priorité
- **Implémentation** : Liste chaînée pour la file de priorité
- **Complexité** : O(V²) avec tableau simple
- **Usage** : Graphes sans poids négatifs
- **Critère** : Minimisation de la latence

#### Bellman-Ford
- **Implémentation** : Relaxation itérative
- **Complexité** : O(V×E)
- **Usage** : Graphes avec poids négatifs possibles
- **Détection** : Cycles négatifs
- **Avantage** : Plus robuste, détecte les anomalies

#### Analyse Comparative
- Comparaison temps d'exécution
- Comparaison résultats
- Cas d'usage recommandés

### 2.2 Plus Court Chemin avec Contraintes (Backtracking)

**Objectif** : Trouver un chemin minimisant la latence AVEC contraintes :

✅ **Contraintes implémentées** :
1. Bande passante minimale garantie
2. Coût maximal autorisé
3. Passage obligatoire par certains nœuds
4. Exclusion de nœuds non-sécurisés (sécurité minimale)

**Technique** :
- Backtracking avec élagage
- Exploration exhaustive contrôlée
- Vérification des contraintes à chaque étape

**Complexité** : O(V!) dans le pire cas, optimisé par élagage

### 2.3 K Plus Courts Chemins (Diviser pour Régner)

**Objectif** : Trouver les K meilleurs chemins alternatifs

✅ **Fonctionnalités** :
- Décomposition récursive du problème
- Tri des chemins par coût
- Élimination des doublons
- Analyse de la redondance du réseau

**Métriques** :
- Taux de redondance (arêtes communes/arêtes totales)
- Distribution des coûts
- Chemins disjoints vs chemins avec arêtes communes

---

## Module 3 : Détection d'Anomalies et Analyse de Sécurité

### 3.1 Détection de Cycles (DFS/BFS)

#### DFS (Depth-First Search)
✅ **Implémentation** :
- Détection de cycles dirigés
- Identification des boucles de routage
- Détection de cycles suspects (attaques par redirection)

#### BFS (Breadth-First Search)
✅ **Implémentation** :
- Tri topologique
- Vérification d'acyclicité
- Détection de dépendances circulaires

### 3.2 Points de Vulnérabilité

#### Points d'Articulation (Nœuds Critiques)
✅ **Algorithme** : DFS modifié avec calcul de low/disc
- Identifie les nœuds dont la suppression déconnecte le graphe
- Critique pour la planification de redondance

#### Ponts (Arêtes Critiques)
✅ **Algorithme** : Extension de l'algorithme des points d'articulation
- Identifie les arêtes critiques
- Analyse de la connectivité

### 3.3 Composantes Fortement Connexes

#### Algorithme de Tarjan
✅ **Implémentation complète** :
- Complexité : O(V+E)
- Une seule passe DFS
- Utilisation d'une pile
- Calcul des composantes fortement connexes

**Applications** :
- Identification de sous-réseaux isolés
- Analyse de la fragmentation du réseau
- Détection de zones autonomes

---

## Module 4 : Gestion des Files de Paquets

### 4.1 File de Priorité (Liste Doublement Chaînée)

✅ **Structure** :
```c
typedef struct Paquet {
    int id;
    int priorite;
    int taille;
    double temps_arrivee;
    double temps_traitement;
    struct Paquet* suivant;
    struct Paquet* precedent;
} Paquet;
```

✅ **Opérations** :
- `enqueue` : Insertion avec priorité - O(n)
- `dequeue` : Extraction du paquet prioritaire - O(1)
- `peek` : Consultation sans suppression - O(1)

### 4.2 Simulation de Flux de Paquets

✅ **Statistiques calculées** :
1. **Temps moyen d'attente** : Moyenne des délais de traitement
2. **Taux de perte** : Paquets perdus / Paquets envoyés
3. **Débit effectif** : Octets transmis / Temps (Mbps)

✅ **Paramètres de simulation** :
- Nombre de paquets
- Capacité de la file
- Distribution des priorités (aléatoire)
- Distribution des tailles (64-1500 octets)

**Alertes** :
- Taux de perte > 10% : ALERTE
- Taux de perte > 5% : ATTENTION
- Taux de perte < 5% : OK

---

## Complexités Algorithmiques

| Algorithme | Complexité Temporelle | Complexité Spatiale |
|------------|----------------------|---------------------|
| Dijkstra | O(V²) | O(V) |
| Bellman-Ford | O(V×E) | O(V) |
| K Plus Courts | O(K×V!) optimisé | O(K×V) |
| Backtracking Contraintes | O(V!) avec élagage | O(V) |
| DFS Cycles | O(V+E) | O(V) |
| Tarjan | O(V+E) | O(V) |
| Points Articulation | O(V+E) | O(V) |
| File Priorité enqueue | O(n) | O(1) |
| File Priorité dequeue | O(1) | O(1) |

---

## Structure des Fichiers

```
src/
├── graphe.c/h                  # Structure de graphe
├── graphe_operations.c         # Opérations avancées
├── liste_chainee.c/h           # Liste chaînée simple
├── dijkstra.c/h                # Algorithme de Dijkstra
├── bellman_ford.c/h            # Algorithme de Bellman-Ford
├── k_plus_courts.c/h           # K plus courts chemins
├── backtracking.c/h            # Backtracking basique
├── routage_contraintes.c/h     # Routage avec contraintes
├── detection_cycles.c/h        # Détection de cycles DFS/BFS
├── tarjan.c/h                  # Algorithme de Tarjan
├── securite.c/h                # Analyse de sécurité
├── file_priorite.c/h           # File de priorité doublement chaînée
├── simulation_paquets.c/h      # Simulation de flux
├── utils.c/h                   # Fonctions utilitaires
└── main_new.c                  # Programme principal complet
```

---

## Compilation et Exécution

### Compilation
```bash
make clean
make
```

### Exécution
```bash
./netflow
```

### Tests
```bash
make tests
./tests_unitaires
```

---

## Fonctionnalités Avancées

### ✅ Implémenté
- [x] Graphes avec 4 pondérations
- [x] Dijkstra avec file de priorité
- [x] Bellman-Ford avec détection cycles négatifs
- [x] K plus courts chemins
- [x] Routage avec contraintes multiples
- [x] Détection cycles DFS/BFS
- [x] Points d'articulation
- [x] Algorithme de Tarjan (composantes FC)
- [x] File de priorité doublement chaînée
- [x] Simulation de flux avec statistiques
- [x] Analyse comparative d'algorithmes

### 🔄 Extensions Possibles
- [ ] Algorithme de Ford-Fulkerson (flux maximum)
- [ ] Algorithme de Kosaraju (alternative à Tarjan)
- [ ] Optimisation Dijkstra avec tas de Fibonacci
- [ ] Interface graphique
- [ ] Export résultats JSON/XML
- [ ] Visualisation graphique du réseau

---

## Cas d'Usage

### 1. Planification de Réseau
- Identifier les chemins optimaux
- Analyser la redondance
- Détecter les points critiques

### 2. Sécurité Réseau
- Identifier les vulnérabilités
- Détecter les cycles suspects
- Analyser la fragmentation

### 3. Optimisation de Performance
- Simuler le flux de paquets
- Calculer les statistiques de performance
- Comparer différentes stratégies de routage

### 4. Maintenance Préventive
- Identifier les nœuds/arêtes critiques
- Planifier la redondance
- Anticiper les pannes

---

## Auteurs
Projet NetFlow Optimizer & Security Analyzer

## Licence
Projet académique - Usage éducatif

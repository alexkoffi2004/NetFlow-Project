# Nom de l'exécutable
EXEC = projetnetflow

# Options de compilation (-Wall affiche tous les avertissements pour t'aider à coder proprement)
CFLAGS = -Wall

# Liste des fichiers sources
SRC = main.c graphe.c file_paquets.c

# Règle par défaut : compile tout le projet
all:
	gcc $(CFLAGS) $(SRC) -o $(EXEC)

# Règle pour nettoyer le dossier (supprime l'exécutable)
clean:
	rm -f $(EXEC)

# Règle pour lancer le programme après la compilation
run: all
	./$(EXEC)
	
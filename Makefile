CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 
TARGET = netflow
TEST_TARGET = tests_unitaires

SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
DOCS_DIR = docs

SOURCES = $(SRC_DIR)/graphe.c \
          $(SRC_DIR)/liste_chainee.c \
          $(SRC_DIR)/dijkstra.c \
          $(SRC_DIR)/backtracking.c \
          $(SRC_DIR)/securite.c \
          $(SRC_DIR)/utils.c \
          $(SRC_DIR)/main.c

TEST_SOURCES = $(SRC_DIR)/graphe.c \
               $(SRC_DIR)/liste_chainee.c \
               $(SRC_DIR)/dijkstra.c \
               $(SRC_DIR)/backtracking.c \
               $(SRC_DIR)/securite.c \
               $(SRC_DIR)/utils.c \
               $(TEST_DIR)/tests_unitaires.c

OBJECTS = $(SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Compilation terminee: $(TARGET)"

tests: $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $^ $(LDFLAGS)
	@echo "Compilation des tests terminee: $(TEST_TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

run-tests: tests
	./$(TEST_TARGET)

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o
	rm -f $(TARGET) $(TEST_TARGET)
	@echo "Nettoyage termine"

rebuild: clean all

help:
	@echo "Makefile pour le projet NetFlow"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  make          - Compile le programme principal"
	@echo "  make all      - Compile le programme principal"
	@echo "  make tests    - Compile les tests unitaires"
	@echo "  make run      - Compile et execute le programme"
	@echo "  make run-tests- Compile et execute les tests"
	@echo "  make clean    - Supprime les fichiers compiles"
	@echo "  make rebuild  - Nettoie et recompile"
	@echo "  make help     - Affiche cette aide"

.PHONY: all tests run run-tests clean rebuild help

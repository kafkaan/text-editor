CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I./include -D_GNU_SOURCE
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Liste des fichiers source
SRCS = $(wildcard $(SRCDIR)/*.c)
# Génération des noms des fichiers objets
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Nom de l'exécutable
TARGET = $(BINDIR)/kirbytext

# Règle par défaut
all: directories $(TARGET)

# Création des répertoires
directories:
	mkdir -p $(OBJDIR) $(BINDIR)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compilation des fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean directories

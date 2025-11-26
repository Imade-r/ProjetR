CC = gcc
CFLAGS = -Wall -std=c99 -Wno-missing-braces -I src/core -I src/players -I src/bonus -I src/ui -I C:/raylib/raylib/src
LDFLAGS = -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm

# Liste de tous les fichiers sources
SRCS = src/core/main.c \
       src/core/assets.c \
       src/core/map.c \
       src/core/game.c \
       src/players/player.c \
       src/bonus/bonus.c \
       src/ui/menu.c

# Nom de l'exécutable dans le dossier game/
TARGET = game/BlastBrawl.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	if not exist game mkdir game
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	if exist game rmdir /s /q game

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -pedantic -Iinclude

SRC = \
    src/main.c \
    \
    src/game/patchword_cli.c \
    src/game/patchword_game.c \
	src/game/patchword_starter.c\
    \
    src/distributed/coordinator.c \
    src/distributed/worker.c \
    src/distributed/worker_logic.c \
    \
    src/dict/dictionary.c \
    src/dict/loader.c \
    \
    src/util/fd.c \
	src/util/io.c \
    src/util/ipc.c \
	src/util/log.c \
    src/util/proc.c \
    src/util/signals.c \
	src/util/str.c \
    \
    src/lib/protocol.c

OBJ = $(SRC:.c=.o)

BIN = patchword

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)

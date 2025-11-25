CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -pedantic -Iinclude

SRC = src/main.c \
      src/lib/protocol.c \
      src/lib/util.c \
      src/coord/coordinator.c \
      src/worker/worker.c

OBJ = $(SRC:.c=.o)

BIN = patchword

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)

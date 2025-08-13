CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc -D_POSIX_C_SOURCE=200809L
SRC = src/main.c src/filesystem.c src/compression.c src/tree.c src/file_loader.c
OBJ = $(SRC:.c=.o)
EXEC = battlefs

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
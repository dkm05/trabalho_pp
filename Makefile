CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
SRC = main.c funcoes.c
OBJ = ${SRC:.c=.o}


main: main.o funcoes.o
	$(CC) $(CFLAGS) -o bin/$@ obj/main.o obj/funcoes.o

main.o: src/main.c
	$(CC) $(CFLAGS) -o obj/$@ -c $<

funcoes.o: src/funcoes.c
	$(CC) $(CFLAGS) -o obj/$@ -c $<

.PHONY: clean
clean:
	rm obj/*.o bin/main

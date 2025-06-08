CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
SRC = main.c hashmap.c
OBJ = ${SRC:.c=.o}


main: main.o hashmap.o vetor.o
	$(CC) $(CFLAGS) -o bin/$@ obj/main.o obj/hashmap.o obj/vetor.o

main.o: src/main.c
	$(CC) $(CFLAGS) -o obj/$@ -c $<

vetor.o: src/vetor.c
	$(CC) $(CFLAGS) -o obj/$@ -c $<

hashmap.o: src/hashmap.c
	$(CC) $(CFLAGS) -o obj/$@ -c $<

.PHONY: clean
clean:
	rm obj/*.o bin/main

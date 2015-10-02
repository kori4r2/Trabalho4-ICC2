CFLAGS = -lm -g -Wall

all: aux main
	gcc -o program *.o $(CFLAGS)

aux:
	gcc $(CFLAGS) -c schema.c
	gcc $(CFLAGS) -c my_strings.c

main:
	gcc $(CFLAGS) -c main.c

clean:
	rm -f *.o
	rm -f program

run:
	./program

.zip:
	zip trabalho4.zip *.c *.h Makefile
debug:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < case.in

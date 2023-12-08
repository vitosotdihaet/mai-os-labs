CC = gcc
CFLAGS = -Wall -Wextra
OPT = -O0

main:
	$(CC) $(CFLAGS) $(OPT) -o main.out main.c -lm -lpthread

debug:
	$(CC) $(CFLAGS) -g -o main.out main.c string.c -lm

clear:
	rm -f main.out string.o

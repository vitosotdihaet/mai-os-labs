CC = gcc
CFLAGS = -Wall -Wextra

main: string.o
	$(CC) $(CFLAGS) -o main.out main.c string.o -lm

debug:
	$(CC) $(CFLAGS) -g -o main.out main.c string.c -lm

string:
	$(CC) $(CFLAGS) -c string.c

clear:
	rm -f main.out string.o
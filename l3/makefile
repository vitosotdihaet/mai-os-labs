CC = gcc
CFLAGS = -Wall -Wextra

main: string.o processor
	$(CC) $(CFLAGS) -g -o main.out main.c string.o -lrt

processor: string.o
	$(CC) $(CFLAGS) -g -o processor.out processor.c string.o -lrt

string:
	$(CC) $(CFLAGS) -g -c string.c

clear:
	rm -f *.o *.out *.txt
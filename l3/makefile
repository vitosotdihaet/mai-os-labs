CC = gcc
CFLAGS = -Wall -Wextra

main: string.o processor
	$(CC) $(CFLAGS) -o main.out main.c string.o
	./main.out

processor: string.o
	$(CC) $(CFLAGS) -o processor.out processor.c string.o

string:
	$(CC) $(CFLAGS) -c string.c

clear:
	rm -f *.o *.out *.txt
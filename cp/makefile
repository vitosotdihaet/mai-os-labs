CC = gcc
CFLAGS = -Wall -Wextra -O3

main: binary_allocator buddy_allocator shared main.c
	$(CC) $(CFLAGS) -o main.out main.c binary_allocator.o buddy_allocator.o shared.o

binary_allocator: binary_allocator.c
	$(CC) $(CFLAGS) -c binary_allocator.c

buddy_allocator: buddy_allocator.c
	$(CC) $(CFLAGS) -c buddy_allocator.c

shared: shared.c
	$(CC) $(CFLAGS) -c shared.c

clear:
	rm -f *.out *.o
CC = gcc
CFLAGS = -Wall -Wextra

main: binary_allocator buddy_allocator main.c
	$(CC) $(CFLAGS) -g -o main.out main.c binary_allocator.o buddy_allocator.o

binary_allocator: binary_allocator.c
	$(CC) $(CFLAGS) -g -c binary_allocator.c

buddy_allocator: buddy_allocator.c
	$(CC) $(CFLAGS) -g -c buddy_allocator.c

clear:
	rm -f *.out *.o
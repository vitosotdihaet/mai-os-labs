CC = gcc
CFLAGS = -Wall -Wextra

LIB_DIR = /usr/local/lib

main: impl1 impl2
	$(CC) $(CFLAGS) -o main.out main.c -ldl

regular_main_impl1: impl1
	$(CC) $(CFLAGS) -o regular_main1.out regular_main.c impl1.o

regular_main_impl2: impl2
	$(CC) $(CFLAGS) -o regular_main2.out regular_main.c impl2.o

impl1: lib1
	$(CC) $(CFLAGS) -shared -o libimpl1.so impl1.o

impl2: lib2
	$(CC) $(CFLAGS) -shared -o libimpl2.so impl2.o

lib1:
	$(CC) $(CFLAGS) -c impl1.c -fPIC

lib2:
	$(CC) $(CFLAGS) -c impl2.c -fPIC

clear:
	rm -f *.out *.so *.o

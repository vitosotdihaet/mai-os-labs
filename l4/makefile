CC = gcc
CFLAGS = -Wall -Wextra

LIB_DIR = /usr/local/lib

main: prime rebase
	$(CC) $(CFLAGS) -o main.out main.c -L. -lprime -lrebase

regular_main: prime rebase
	$(CC) $(CFLAGS) -o regular_main.out main.c libprime.so librebase.so

prime:
	$(CC) $(CFLAGS) -shared -o libprime.so prime.c -fPIC
	sudo cp libprime.so $(LIB_DIR)

rebase:
	$(CC) $(CFLAGS) -shared -o librebase.so rebase.c -fPIC
	sudo cp librebase.so $(LIB_DIR)

clear:
	rm -f *.out *.dll *.so
	sudo rm -f $(LIB_DIR)/libprime.so
	sudo rm -f $(LIB_DIR)/librebase.so

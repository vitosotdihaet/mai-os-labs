CC = g++
FLAGS = -Wall -Wextra -ggdb
ZMQ = `pkg-config --libs libzmq`


main: main.cpp btree.hpp child
	$(CC) $(FLAGS) -o main.out main.cpp btree.hpp $(ZMQ)

child: child.cpp
	$(CC) $(FLAGS) -o child.out child.cpp $(ZMQ)

zmqtest: zmq_client.cpp zmq_server.cpp
	$(CC) $(FLAGS) -o zmq_client.out zmq_client.cpp $(ZMQ)
	$(CC) $(FLAGS) -o zmq_server.out zmq_server.cpp $(ZMQ)

clean:
	rm -rf *.out
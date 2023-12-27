#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>


int running = 1;

int main() {
    // Prepare context and socket
    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("ipc:///tmp/lab5_5555");

    std::cout << "Server listening on icp 5555..." << std::endl;


    while (running) {
        zmq::message_t request;

        // Wait for the next request from the client
        auto _ = socket.recv(request, zmq::recv_flags::none);
        std::string message = std::string(static_cast<char*>(request.data()), request.size());
        std::cout << "Received request: " << message << std::endl;

        // Send "World" back to the client
        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        socket.send(reply, zmq::send_flags::none);
    }

    socket.close();

    return 0;
}

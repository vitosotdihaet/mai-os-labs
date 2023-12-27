#include <iostream>
#include <string>
#include <map>

#include <zmq.hpp>


static std::map<std::string, int> var_map;

std::string next(std::string* s) {
    size_t i = 0;
    for (; i < s->size() && (*s)[i] != ' ' && (*s)[i] != '\n'; ++i) {}

    std::string result = s->substr(0, i);
    *s = s->substr(i + 1, s->size() - (i + 1));

    return result;
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage:\n\t" << argv[0] << " <socket_path>\n";
        return 0;
    }

    zmq::context_t ctx;
    zmq::socket_t socket(ctx, ZMQ_REP);
    socket.bind(argv[1]);

    bool run = true;

    while (run) {
        zmq::message_t request;

        socket.recv(request, zmq::recv_flags::none);

        std::string s = std::string(static_cast<char*>(request.data()), request.size());
        std::cout << "Received message: " << s;
        std::string to_send;

        std::string command = next(&s);

        if (command[0] == 'g') {
            std::string var = next(&s);
            int value = 0;

            if (var_map.find(var) != var_map.end()) {
                value = var_map.at(var);
                to_send += var + ": " + std::to_string(value);
            } else {
                to_send = var + " not found";
            }
        } else if (command[0] == 's') {
            std::string var = next(&s);
            int value = std::stoi(next(&s));

            var_map.insert(std::pair(var, value));
            to_send = var + " is set to " + std::to_string(value);
        } else if (isdigit(command[0])) {
            // getting string: "1 2 3 4 <command>" => send to 1: "2 3 4 <command>"
            // "3 4 <command>" -> "4 <command>" -> "<command>"

            std::string next_num = command;
            std::cout << "next_num = " << next_num << '\n';

            zmq::socket_t next_socket = zmq::socket_t(ctx, ZMQ_REQ);
            next_socket.connect("icp:///tmp/lab5_" + next_num);

            std::string next_next = next(&s);
            if (isdigit(next_next[0])) {
                next_socket.send(zmq::buffer("n " + s), zmq::send_flags::none);
            } else {
                next_socket.send(zmq::buffer(s), zmq::send_flags::none);
            }

            std::cout << "MESSAGE SENT!\n";

            zmq::message_t request;
            // TODO: if wait time is more than 100ms => Err
            next_socket.recv(request, zmq::recv_flags::none);
            to_send = std::string(static_cast<char*>(request.data()), request.size());
        } else {
            run = false;
        }

        socket.send(zmq::buffer(to_send), zmq::send_flags::none);
    }

    return 0;
}

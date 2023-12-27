#include <iostream>
#include <string>
#include <map>

#include <zmq.hpp>


static std::map<std::string, int> var_map;

std::string next(std::string* s) {
    int i = 0;
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

    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind(argv[1]);

    bool run = true;

    while (run) {
        zmq::message_t request;

        std::cout << "Waiting for a msg...\n";
        socket.recv(request, zmq::recv_flags::none);

        std::string s = std::string(static_cast<char*>(request.data()), request.size());
        std::string to_send;

        char command = next(&s)[0];
        std::string var = next(&s);

        if (command == 'g') {
            int value = 0;
            if (var_map.find(var) != var_map.end()) {
                value = var_map.at(var);
                to_send += var + ": " + std::to_string(value);
            } else {
                to_send = var + " not found";
            }
        } else if (command == 's') {
            int value = std::stoi(next(&s));
            var_map.insert(std::pair(var, value));
            to_send = var + " is set to " + std::to_string(value);
        } else {
            // TODO: queue messages to next child
            run = false;
        }

        socket.send(zmq::buffer(to_send), zmq::send_flags::none);
    }

    return 0;
}

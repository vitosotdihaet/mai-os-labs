#include <iostream>
#include <unistd.h>
#include <string>
#include <optional>
#include <tuple>
#include <thread>

#include <zmq.hpp>

#include "btree.hpp"


#define crash_on(boolean) if (boolean) { fprintf(stderr, "Error at file %s, line %d\n", __FILE__, __LINE__); }
#define handle(i) crash_on(i == -1)


static ProcessTree processes = ProcessTree();

std::string next(std::string* s) {
    int i = 0;
    for (; i < s->size() && (*s)[i] != ' ' && (*s)[i] != '\n'; ++i) {}

    std::string result = s->substr(0, i);
    *s = s->substr(i + 1, s->size());

    return result;
}

int main() {
    std::string line = std::string();

    zmq::context_t ctx;
    zmq::socket_t *root_socket = new zmq::socket_t(ctx, ZMQ_REQ);
    // root_socket->connect("ipc:///tmp/lab5_0");

    processes.insert(std::tuple(0, root_socket, true));

    std::cout << "> ";
    while (getline(std::cin, line)) {
        line.push_back('\n');

        std::string command = next(&line);

        if (command == std::string("create")) {
            int child_id = std::stoi(next(&line));

            if (processes.is_in_tree(child_id)) {
                std::cout << "Error: Child with id = " << child_id << " already exists!\n";
            } else {
                std::string socket_path = "ipc:///tmp/lab5_" + std::to_string(child_id);
                zmq::socket_t *socket = new zmq::socket_t(ctx, ZMQ_REQ);
                socket->connect(socket_path);

                pid_t child_pid = fork();

                if (child_pid == -1) {
                    std::cout << "Error: Could not fork current process!\n";
                } else if (child_pid == 0) {
                    char *socket_path_c_str = (char*) calloc(socket_path.size(), sizeof(char));
                    memcpy(socket_path_c_str, socket_path.c_str(), socket_path.size() * sizeof(char));

                    char *const argv[] = { "child.out", socket_path_c_str, NULL };
                    if (execv(argv[0], argv) == -1) std::cout << "Error: Could not create a child process!\n";
                } else {
                    std::cout << "Ok: child pid is " << child_pid << '\n';
                    processes.insert(std::tuple(child_id, socket, true));
                }
            }
        } else if (command == std::string("exec")) {
            int child_id = std::stoi(next(&line));

            std::string var = next(&line);

            if (line.size() > 0) {
                // set variable in child process
                std::optional<std::tuple<int, zmq::socket_t*, bool>> data = processes.get_by_id(child_id);

                if (!data) {
                    std::cout << "Error: No child with id = " << child_id << "!\n";
                    std::cout << "> ";
                    continue;
                }

                std::string value = next(&line);

                std::string msg = "s " + var + " " + value;

                zmq::socket_t *socket = std::get<1>(data.value());

                socket->send(zmq::buffer(msg), zmq::send_flags::none);

                std::thread wait(
                    [&socket]() {
                        zmq::message_t request;
                        socket->recv(request, zmq::recv_flags::none);

                        std::string message = std::string(static_cast<char*>(request.data()), request.size());
                        std::cout << "Ok: " << std::get<0>(data.value()) << ": " << message << std::endl;
                    }
                );
                wait.join();
            } else {
                // find variable in child process
                std::optional<std::tuple<int, zmq::socket_t*, bool>> data = processes.get_by_id(child_id);

                if (!data) {
                    std::cout << "Error: No child with id = " << child_id << "!\n";
                    std::cout << "> ";
                    continue;
                }

                std::string msg = "g " + var;

                zmq::socket_t *socket = std::get<1>(data.value());

                socket->send(zmq::buffer(msg), zmq::send_flags::none);

                std::thread wait(
                    [&socket]() {
                        zmq::message_t request;
                        socket->recv(request, zmq::recv_flags::none);

                        std::string message = std::string(static_cast<char*>(request.data()), request.size());
                        std::cout << "Received request in main: " << message << std::endl;
                    }
                );
                wait.join();
            }
        } else if (command == std::string("ping")) {
            int child_id = std::stoi(next(&line));

            if (!processes.is_in_tree(child_id)) std::cout << "Error: Child with id = " << child_id << " does not exist!\n";
            // TODO: check if socket is broken (if the file of socket exists | send a message and wait a bit)
            else std::cout << "Ok: " << child_id << '\n';
        } else if (command == std::string("dbg")) {
            processes.print_levels();
        } else if (command == std::string("exit")) {
            break;
        }

        std::cout << "> ";
    }
}
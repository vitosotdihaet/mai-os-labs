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

zmq::context_t ctx;
zmq::socket_t root_socket = zmq::socket_t(ctx, ZMQ_REQ);


void send_msg() {
    zmq::message_t request;

    root_socket.recv(request, zmq::recv_flags::none);

    std::string message = std::string(static_cast<char*>(request.data()), request.size());
    std::cout << message << std::endl;
}

std::string next(std::string* s) {
    unsigned long i = 0;
    for (; i < s->size() && (*s)[i] != ' ' && (*s)[i] != '\n'; ++i) {}

    std::string result = s->substr(0, i);
    *s = s->substr(i + 1, s->size());

    return result;
}


int main() {
    std::string line = std::string();
    root_socket.connect("ipc:///tmp/lab5_0");

    pid_t child_pid = fork();

    if (child_pid == -1) {
        std::cout << "Error: Could not fork current process!\n";
        return 1;
    } else if (child_pid == 0) {
        const char *argv[] = { "./child.out", "ipc:///tmp/lab5_0", NULL };
        if (execv(argv[0], const_cast<char* const*>(argv)) == -1) std::cout << "Error: Could not create a child process!\n";
    }

    processes.insert(std::tuple(0, true));

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

                pid_t child_pid = fork();

                if (child_pid == -1) {
                    std::cout << "Error: Could not fork current process!\n";
                } else if (child_pid == 0) {
                    const char *argv[] = { "./child.out", socket_path.c_str(), NULL };
                    if (execv(argv[0], const_cast<char* const*>(argv)) == -1) std::cout << "Error: Could not create a child process!\n";
                } else {
                    std::cout << "Ok: child pid is " << child_pid << '\n';
                    processes.insert(std::tuple(child_id, true));
                }
            }
        } else if (command == std::string("exec")) {
            int child_id = std::stoi(next(&line));

            std::string var = next(&line);

            if (line.size() > 0) {
                // set variable in child process
                std::optional<std::vector<int>> path = processes.get_path_to(child_id);

                if (!path) {
                    std::cout << "Error: No child with id = " << child_id << "!\n";
                    std::cout << "> ";
                    continue;
                }

                std::string value = next(&line);

                std::string msg_command = "s " + var + " " + value + '\n';
                std::string msg;

                for (unsigned long i = 1; i < path.value().size(); ++i) {
                    int id = path.value()[i];
                    msg += std::to_string(id) + ' ';
                }
                msg += msg_command;

                root_socket.send(zmq::buffer(msg), zmq::send_flags::none);

                std::thread wait(send_msg);
                wait.join();
            } else {
                // find variable in child process
                std::optional<std::vector<int>> path = processes.get_path_to(child_id);

                if (!path) {
                    std::cout << "Error: No child with id = " << child_id << "!\n";
                    std::cout << "> ";
                    continue;
                }

                std::string msg_command = "g " + var + '\n';
                std::string msg;

                for (unsigned long i = 1; i < path.value().size(); ++i) {
                    int id = path.value()[i];
                    msg += std::to_string(id) + ' ';
                }
                msg += msg_command;

                root_socket.send(zmq::buffer(msg), zmq::send_flags::none);

                std::thread wait(send_msg);
                wait.join();
            }
        } else if (command == std::string("ping")) {
            int child_id = std::stoi(next(&line));

            // TODO: check if socket is broken (if the file of socket exists | send a message and wait a bit)            
            std::optional<std::vector<int>> path = processes.get_path_to(child_id);

            if (!path) {
                std::cout << "Error: No child with id = " << child_id << "!\n";
                std::cout << "> ";
                continue;
            }

            std::string msg_command = "ping\n";
            std::string msg;

            for (unsigned long i = 1; i < path.value().size(); ++i) {
                int id = path.value()[i];
                msg += std::to_string(id) + ' ';
            }
            msg += msg_command;

            root_socket.send(zmq::buffer(msg), zmq::send_flags::none);
            
            std::thread wait(send_msg);
            wait.join();
        } else if (command == std::string("dbg")) {
            processes.print_levels();
        } else if (command == std::string("exit")) {
            break;
        }

        std::cout << "> ";
    }

    return 0;
}
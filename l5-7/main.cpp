#include <iostream>
#include <unistd.h>
#include <string>
#include <optional>
#include <tuple>

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

    processes.insert(std::tuple(0, getpid(), 0));

    std::cout << "> ";
    while (getline(std::cin, line)) {
        line.push_back('\n');

        std::string command = next(&line);

        if (command == std::string("create")) {
            int child_id = std::stoi(next(&line));

            if (processes.is_in_tree(child_id)) {
                std::cout << "Error: Child with id = " << child_id << " already exists!\n";
            } else {
                int p[2];
                handle(pipe(p));

                pid_t child_pid = fork();
                if (child_pid == -1) {
                    std::cout << "Error: Could not fork current process!\n";
                } else if (child_pid == 0) {
                    handle(close(p[1]));
                    if (dup2(p[0], STDIN_FILENO) == -1) {
                        std::cout << "Error: Could not dup2!\n";
                        std::cout << "> ";
                        continue;
                    }

                    char *const argv[] = { "child.out", NULL };
                    if (execv(argv[0], argv) == -1) std::cout << "Error: Could not create a child process!\n";
                } else {
                    handle(close(p[0]));

                    std::cout << "Ok: child pid is " << child_pid << '\n';
                    processes.insert(std::tuple(child_id, child_pid, p[1]));
                }
            }
        } else if (command == std::string("exec")) {
            int child_id = std::stoi(next(&line));

            std::string var = next(&line);

            if (line.size() > 0) {
                // set variable in child process
                std::optional<std::tuple<int, pid_t, int>> data = processes.get_by_id(child_id);

                if (!data) {
                    std::cout << "Error: No child with id = " << child_id << "!\n";
                    std::cout << "> ";
                    continue;
                }

                std::string value = next(&line);

                int count = 1 + var.size() + 1 + value.size() + 1;
                char *buf = (char*) calloc(count, sizeof(char));
                buf[0] = 's';
                for (int i = 0; i < var.size(); ++i) buf[i + 1] = var[i];
                buf[1 + var.size()] = ' ';
                for (int i = 0; i < value.size(); ++i) buf[i + 1 + var.size() + 1] = value[i];
                buf[count - 1] = '\n';

                if (write(std::get<2>(data.value()), buf, count * sizeof(char)) != count * sizeof(char)) {
                    std::cout << "Error: Writing to child with id = " << child_id << " failed!\n";
                }
            } else {
                // find variable in child process
                std::optional<std::tuple<int, pid_t, int>> data = processes.get_by_id(child_id);

                if (!data) {
                    std::cout << "Error: No child with id = " << child_id << "!\n";
                    std::cout << "> ";
                    continue;
                }

                int count = var.size() + 2;
                char *buf = (char*) calloc(count, sizeof(char));
                buf[0] = 'g';
                for (int i = 0; i < var.size(); ++i) buf[i + 1] = var[i];
                buf[count - 1] = '\n';

                if (write(std::get<2>(data.value()), buf, count * sizeof(char)) != count * sizeof(char)) {
                    std::cout << "Error: Writing to child with id = " << child_id << " failed!\n";
                }
            }
        } else if (command == std::string("ping")) {
            int child_id = std::stoi(next(&line));

            if (!processes.is_in_tree(child_id)) std::cout << "Error: Child with id = " << child_id << " does not exist!\n";
            else std::cout << "Ok: " << child_id << '\n';
        } else if (command == std::string("dbg")) {
            processes.print_levels();
        }

        std::cout << "> ";
    }
}
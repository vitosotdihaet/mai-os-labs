#include <iostream>
#include <string>
#include <map>


static std::map<std::string, int> var_map;

std::string next(std::string* s) {
    int i = 0;
    for (; i < s->size() && (*s)[i] != ' ' && (*s)[i] != '\n'; ++i) {}

    std::string result = s->substr(0, i);
    *s = s->substr(i + 1, s->size() - (i + 1));

    return result;
}

int main() {
    bool run = true;

    std::string s = std::string();
    while (run && getline(std::cin, s)) {
        s.push_back('\n');

        std::string no_command = s.substr(1, s.size() - 1);
        std::string var = next(&no_command);

        if (s[0] == 'g') {
            int value = 0;
            if (var_map.find(var) != var_map.end()) {
                value = var_map.at(var);
                std::cout << "Ok: " << var << ": " << value << '\n';
            } else {
                std::cout << "Ok: " << var << ": not found\n";
            }
        } else if (s[0] == 's') {
            int value = std::stoi(next(&no_command));
            var_map.insert(std::pair(var, value));
            std::cout << "Ok: " << var << " is set to " << value << '\n';
        } else {
            run = false;
        }
    }
}
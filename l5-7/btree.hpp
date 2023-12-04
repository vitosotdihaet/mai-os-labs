#include <iostream>
#include <vector>
#include <tuple>
#include <optional>


int pow2(int n) {
    int result = 1;

    for (int i = 0; i < n; ++i) {
        result *= 2;
    }

    return result;
}


class ProcessTree {
private:
    struct Node {
        std::tuple<int, pid_t, int> data;
        Node* left;
        Node* right;

        Node(std::tuple<int, pid_t, int> data): data(data), left(nullptr), right(nullptr) {}
    };

    Node* root;
    std::vector<std::vector<Node*>> levels;

    void free_(Node* n) {
        if (n->left != nullptr) {
            free_(n->left);
            free(n->left);
        }

        if (n->right != nullptr) {
            free_(n->right);
            free(n->right);
        }
    }

public:
    ProcessTree(): root(nullptr) { levels = std::vector<std::vector<Node*>>(0); }

    void insert(std::tuple<int, pid_t, int> data) {
        if (root == nullptr) {
            root = new Node(data);
            levels.push_back(std::vector<Node*>(1, root));
            return;
        }

        int max_height = levels.size();
        for (int i = 0; i < max_height; ++i) {
            if (pow2(i) == levels[i].size()) {
                continue;
            }

            for (int j = 0; j < levels[i - 1].size(); ++j) {
                if (levels[i - 1][j]->left == nullptr) {
                    levels[i - 1][j]->left = new Node(data);
                    levels[i].push_back(levels[i - 1][j]->left);
                    return;
                } else if (levels[i - 1][j]->right == nullptr) {
                    levels[i - 1][j]->right = new Node(data);
                    levels[i].push_back(levels[i - 1][j]->right);
                    return;
                }
            }
        }

        levels[max_height - 1][0]->left = new Node(data);
        levels.push_back(std::vector<Node*>(1, levels[max_height - 1][0]->left));
    }

    bool is_in_tree(int id) {
        for (auto level: levels) {
            for (auto e: level) {
                if (std::get<0>(e->data) == id) return true;
            }
        }

        return false;
    }

    std::optional<std::tuple<int, pid_t, int>> get_by_id(int id) {
        for (auto level: levels) {
            for (auto e: level) {
                if (std::get<0>(e->data) == id) return e->data;
            }
        }

        return std::nullopt;
    }

    void print_levels() {
        for (auto level: levels) {
            for (auto e: level) std::cout << std::get<0>(e->data) << ' ';
            std::cout << '\n';
        }
    }

    ~ProcessTree() {
        if (root != nullptr) free_(root);
    }
};

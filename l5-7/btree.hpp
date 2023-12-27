#include <iostream>
#include <vector>
#include <tuple>
#include <optional>

#include <zmq.hpp>


size_t pow2(size_t n) {
    return 1 << n;
}


class ProcessTree {
private:
    using data_t = std::tuple<int, bool>;
    struct Node {
        // child_id, socket, is active
        data_t data;
        Node* left;
        Node* right;

        Node(data_t data): data(data), left(nullptr), right(nullptr) {}
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

    std::optional<std::vector<int>> _get_path_to(Node *n, int id, std::vector<int> &current_path) {
        if (n == nullptr) return std::nullopt;

        int current_id = std::get<0>(n->data);
        current_path.push_back(current_id);
        
        if (id == current_id) return current_path;
        
        // Traverse the left subtree
        std::optional<std::vector<int>> left = _get_path_to(n->left, id, current_path);
        if (left) {
            return left;
        }

        // Traverse the right subtree
        std::optional<std::vector<int>> right = _get_path_to(n->right, id, current_path);
        if (right) {
            return right;
        }

        // If we didn't find the target node in this subtree, remove the current node from the path
        current_path.pop_back();

        return std::nullopt;
    }


public:
    ProcessTree(): root(nullptr) { levels = std::vector<std::vector<Node*>>(0); }

    void insert(data_t data) {
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

            for (unsigned long j = 0; j < levels[i - 1].size(); ++j) {
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

    std::optional<data_t > get_by_id(int id) {
        for (auto level: levels) {
            for (auto e: level) {
                if (std::get<0>(e->data) == id) return std::make_optional(e->data);
            }
        }

        return std::nullopt;
    }

    std::optional<std::vector<int>> get_path_to(int id) {
        std::vector<int> v;
        return _get_path_to(root, id, v);
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

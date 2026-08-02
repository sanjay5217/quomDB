#pragma once

#include <optional>
#include <random>
#include <vector>

template <typename key_type, typename value_type>
class SkipList {
    public:
    explicit SkipList(int level);
    ~SkipList();
    SkipList(const SkipList& other) = delete;
    SkipList& operator=(const SkipList& other) = delete;

    std::optional<value_type> search(key_type key);
    void insert(key_type key, value_type value);
    bool erase(key_type key);

    private:
    struct Node {
        key_type key;
        value_type value;
        std::vector<Node*> forward;

        Node(key_type key, value_type value, int level)
            : key(key), value(value), forward(level, nullptr) {}
    };

    int max_level;
    int current_level;
    Node* head;

    std::mt19937 rng;
    std::bernoulli_distribution coin;

    int randomLevel();
};

template <typename key_type, typename value_type>
SkipList<key_type, value_type>::SkipList(int level) : 
    max_level(level), 
    current_level(1), 
    rng(std::random_device{}()), 
    coin(0.5) {
    head = new Node(key_type{}, value_type{}, max_level);
}

template <typename key_type, typename value_type>
SkipList<key_type, value_type>::~SkipList() {
    Node* current = head->forward[0];
    while (current != nullptr) {
        Node* next = current->forward[0];
        delete current;
        current = next;
    }
    delete head;
}

template <typename key_type, typename value_type>
int SkipList<key_type, value_type>::randomLevel() {
    int level = 1;
    while (level < max_level && coin(rng)) {
        level++;
    }
    return level;
}

template <typename key_type, typename value_type>
std::optional<value_type> SkipList<key_type, value_type>::search(key_type key) {
    Node* current = head;
    for (int i = current_level - 1; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];
    if (current != nullptr && current->key == key) {
        return current->value;
    }
    return std::nullopt;
}

template <typename key_type, typename value_type>
void SkipList<key_type, value_type>::insert(key_type key, value_type value) {
    std::vector<Node*> update(max_level, head);
    Node* current = head;

    for (int i = current_level - 1; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if (current != nullptr && current->key == key) {
        current->value = value;
        return;
    }

    int new_level = randomLevel();
    if (new_level > current_level) {
        for (int i = current_level; i < new_level; i++) {
            update[i] = head;
        }
        current_level = new_level;
    }

    Node* node = new Node(key, value, new_level);
    for (int i = 0; i < new_level; i++) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
}

template <typename key_type, typename value_type>
bool SkipList<key_type, value_type>::erase(key_type key) {
    std::vector<Node*> update(max_level, head);
    Node* current = head;

    for (int i = current_level - 1; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current == nullptr || current->key != key) {
        return false;
    }

    for (int i = 0; i < current_level; i++) {
        if (update[i]->forward[i] != current) {
            break;
        }
        update[i]->forward[i] = current->forward[i];
    }

    delete current;

    while (current_level > 1 && head->forward[current_level - 1] == nullptr) {
        current_level--;
    }

    return true;
}

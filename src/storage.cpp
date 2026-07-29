#include "../include/storage.hpp"

StorageEngine::StorageEngine() : data{} {}

std::optional<std::string> StorageEngine::get(std::string key) {
    auto it = data.find(key);
    if (it == data.end()) {
        return std::nullopt;
    } return it->second;
}

void StorageEngine::put(const std::string &key, const std::string &value) {
    this->data[key] = value;
}

bool StorageEngine::del(std::string key) {
    return data.erase(key) > 0;
}

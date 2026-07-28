#include "../include/storage.hpp"

StorageEngine::StorageEngine() : data{} {}

std::optional<Value> StorageEngine::get(Key key) {
    auto it = data.find(key);
    if (it == data.end()) {
        return std::nullopt;
    } return it->second;
}

void StorageEngine::put(const Key &key, const Value &value) {
    this->data[key] = value;
}

bool StorageEngine::del(Key key) {
    return data.erase(key) > 0;
}

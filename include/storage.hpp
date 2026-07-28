#pragma once 

#include <iostream>
#include <string>
#include <optional>
#include <unordered_map>

using Key = std::string;
using Value = std::string;

class StorageEngine {
    public: 
    StorageEngine(void);
    std::optional<Value> get(Key key);
    void put(const Key &key, const Value &value);
    bool del(Key key);

    private:
    std::unordered_map<std::string, std::string> data;
};
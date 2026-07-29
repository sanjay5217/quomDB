#pragma once 

#include <iostream>
#include <string>
#include <optional>
#include <unordered_map>

class StorageEngine {
    public: 
    StorageEngine(void);
    std::optional<std::string> get(std::string key);
    void put(const std::string &key, const std::string &value);
    bool del(std::string key);

    private:
    std::unordered_map<std::string, std::string> data;
};


#include "../include/database.hpp"

class Database {
    
    Database::Database() : data{} {}

    std::optional<Value> Database::get(Key key) {
        auto it = data.find(key);
        if (it == data.end()) {
            return std::nullopt;
        } return it->second;      
    }

    void Database::put(const Key &key, const Value &value) {
        this.data[key] = value;
    }

    bool Database::del(Key key) {
        return data.erase(key) > 0;
    }
}


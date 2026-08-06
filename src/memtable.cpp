#include "../include/memtable.hpp"

MemTable::MemTable() : status{MemTableState::ACTIVE} {}

void MemTable::put(std::string key, std::string value) {
    map.insert(key, value);
    current_size += key.size() + value.size();
    if (current_size >= MEMTABLE_SIZE) {
        status = MemTableState::FLUSHING;
    }
}

std::optional<std::string> MemTable::get(std::string key) {
    return map.search(key);
}

bool MemTable::erase(std::string key) {
    std::optional<std::string> existing = map.search(key);
    if (!existing.has_value()) {
        return false;
    }
    current_size -= key.size() + existing->size();
    return map.erase(key);
}

void MemTable::flush() {
    current_size = 0;
    status = MemTableState::FLUSHED;
}
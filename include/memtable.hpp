#pragma once

#include <optional>
#include <string>

#include "skiplist.hpp"

static constexpr size_t LEVEL_SIZE = 16;
static constexpr size_t MEMTABLE_SIZE = 67108864;

enum class MemTableState {
    ACTIVE,
    FLUSHING,
    FLUSHED
};

class MemTable {
    public:
    MemTable(void);
    void put(std::string key, std::string value);
    std::optional<std::string>  get(std::string  key);
    bool erase(std::string  key);

    private:
    std::size_t current_size = 0;
    MemTableState status;
    void flush(void);
    SkipList<std::string, std::string> map{LEVEL_SIZE};
};

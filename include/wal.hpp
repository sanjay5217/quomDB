#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstdint>

#include "storage.hpp"

static constexpr size_t BATCH_SIZE = 10;

enum class CommandType : uint8_t {
    GET = 0,
    PUT = 1,
    DELETE = 2
};

struct Log {
    CommandType command;
    std::string key;
    std::string value;
    uint64_t timestamp;

bool operator==(const Log& other) const {
        return command == other.command &&
               key == other.key &&
               value == other.value &&
               timestamp == other.timestamp;
    }
};

class WriteAheadLog {
    public:
    WriteAheadLog(void);
    WriteAheadLog(const WriteAheadLog& other) = delete;
    WriteAheadLog& operator=(const WriteAheadLog& other) = delete;
    ~WriteAheadLog();

    void append(Log log_entry);
    void replay(void);
    void flush(void);

    std::vector<std::byte> encode(Log log);
    Log decode(const std::vector<std::byte>& log_entry) const;

    private:
    int logfd;
    std::vector<std::vector<std::byte>> bbuffer;
    size_t bbuffer_entries;
};
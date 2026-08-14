#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>

#include "storage.hpp"

enum class CommandType : uint8_t {
    PUT = 0,
    DELETE = 1
};

struct Log {
    uint64_t sequence;
    CommandType command;
    std::string key;
    std::string value;
    uint64_t timestamp;

    bool operator==(const Log& other) const {
        return sequence == other.sequence &&
               command == other.command &&
               key == other.key &&
               value == other.value &&
               timestamp == other.timestamp;
    }
};

class WriteAheadLog {
    private:
    int logfd = -1;

    public:
    WriteAheadLog(void);
    WriteAheadLog(const WriteAheadLog& other) = delete;
    WriteAheadLog& operator=(const WriteAheadLog& other) = delete;
    ~WriteAheadLog();

    void append(Log log_entry);
    void replay(void);
};

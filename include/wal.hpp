#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstdint>

#include "storage.hpp"

static constexpr size_t BATCH_SIZE = 10;
static constexpr size_t SEGMENT_SIZE_LIMIT = 67108864;

enum class CommandType : uint8_t {
    PUT = 0,
    DELETE = 1
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
    private:
    int logfd = -1;
    int segment_index = 0;
    size_t segment_size = 0;
    std::vector<std::vector<std::byte>> bbuffer;
    size_t bbuffer_entries;

    static std::string segmentPath(int index);
    void openSegment(int index);
    static uint32_t crc32(const std::vector<std::byte>& data);

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
};
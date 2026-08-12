#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>

#include "wal.hpp"
#include "wal_utils.hpp"

TEST(WriteAheadLog, EncodeWorks) {
    Log log{1, CommandType::PUT, "key", "value", 1000};

    std::vector<std::byte> bytes = {
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00},
        std::byte{0x83}, std::byte{0x6a}, std::byte{0xbf}, std::byte{0x9b},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0xE8}, std::byte{0x03}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'}, std::byte{'e'}, std::byte{'y'},
        std::byte{'v'}, std::byte{'a'}, std::byte{'l'}, std::byte{'u'}, std::byte{'e'}
    };

    EXPECT_EQ(encode(log), bytes);
}

TEST(WriteAheadLog, DecodeWorks) {
    Log log{1, CommandType::PUT, "key", "value", 1000};

    std::vector<std::byte> bytes = {
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00},
        std::byte{0x83}, std::byte{0x6a}, std::byte{0xbf}, std::byte{0x9b},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0xE8}, std::byte{0x03}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'}, std::byte{'e'}, std::byte{'y'},
        std::byte{'v'}, std::byte{'a'}, std::byte{'l'}, std::byte{'u'}, std::byte{'e'}
    };

    EXPECT_EQ(decode(bytes), log);
}

TEST(WriteAheadLog, EncodeDecodeEmptyKeyAndValue) {
    Log log{1, CommandType::DELETE, "", "", 0};

    std::vector<std::byte> encoded = encode(log);
    Log decoded = decode(encoded);

    EXPECT_EQ(decoded, log);
}

TEST(WriteAheadLog, EncodeDecodeRoundTrip) {
    Log log{1, CommandType::PUT, "hello", "world", 1234567890ULL};

    std::vector<std::byte> encoded = encode(log);
    Log decoded = decode(encoded);

    EXPECT_EQ(decoded, log);
}

TEST(WriteAheadLog, AppendPersistsImmediately) {
    std::remove("data/wal.log");

    WriteAheadLog wal;
    Log entry{1, CommandType::PUT, "key", "value", 100};
    size_t entry_size = encode(entry).size();

    auto file_entry_count = [&]() -> size_t {
        std::ifstream file("data/wal.log", std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return 0;
        }
        return static_cast<size_t>(file.tellg()) / entry_size;
    };

    for (size_t i = 1; i <= 5; i++) {
        wal.append(entry);
        EXPECT_EQ(file_entry_count(), i);
    }

    std::ifstream file("data/wal.log", std::ios::binary);
    std::vector<std::byte> first_entry_bytes(entry_size);
    file.read(reinterpret_cast<char*>(first_entry_bytes.data()), static_cast<std::streamsize>(entry_size));
    EXPECT_EQ(decode(first_entry_bytes), entry);

    std::remove("data/wal.log");
}

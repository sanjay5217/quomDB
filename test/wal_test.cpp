#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>

#include "wal.hpp"

TEST(WriteAheadLog, EncodeWorks) {
    WriteAheadLog wal;

    Log log{CommandType::PUT, "key", "value", 1000};
    
    std::vector<std::byte> bytes = {
        std::byte{0x00},
        std::byte{0x72}, std::byte{0xce}, std::byte{0xfe}, std::byte{0x3e},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0xE8}, std::byte{0x03}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'}, std::byte{'e'}, std::byte{'y'},
        std::byte{'v'}, std::byte{'a'}, std::byte{'l'}, std::byte{'u'}, std::byte{'e'}
    };

    EXPECT_EQ(wal.encode(log), bytes);
}

TEST(WriteAheadLog, DecodeWorks) {
    WriteAheadLog wal;

    Log log{CommandType::PUT, "key", "value", 1000};
    
    std::vector<std::byte> bytes = {
        std::byte{0x00},
        std::byte{0x72}, std::byte{0xce}, std::byte{0xfe}, std::byte{0x3e},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0xE8}, std::byte{0x03}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'}, std::byte{'e'}, std::byte{'y'},
        std::byte{'v'}, std::byte{'a'}, std::byte{'l'}, std::byte{'u'}, std::byte{'e'}
    };

    EXPECT_EQ(wal.decode(bytes), log);
}

TEST(WriteAheadLog, EncodeDecodeEmptyKeyAndValue) {
    WriteAheadLog wal;

    Log log{CommandType::DELETE, "", "", 0};

    std::vector<std::byte> encoded = wal.encode(log);
    Log decoded = wal.decode(encoded);

    EXPECT_EQ(decoded, log);
}

TEST(WriteAheadLog, EncodeDecodeRoundTrip) {
    WriteAheadLog wal;

    Log log{CommandType::PUT, "hello", "world", 1234567890ULL};

    std::vector<std::byte> encoded = wal.encode(log);
    Log decoded = wal.decode(encoded);

    EXPECT_EQ(decoded, log);
}

TEST(WriteAheadLog, AppendandFlushTest) {
    std::remove("data/wal-0.log");

    WriteAheadLog wal;
    Log entry{CommandType::PUT, "key", "value", 100};
    size_t entry_size = wal.encode(entry).size();

    auto file_entry_count = [&]() -> size_t {
        std::ifstream file("data/wal-0.log", std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return 0;
        }
        return static_cast<size_t>(file.tellg()) / entry_size;
    };

    for (int i = 0; i < 4; i++) {
        wal.append(entry);
    }
    EXPECT_EQ(file_entry_count(), 0u);

    for (int i = 0; i < 6; i++) {
        wal.append(entry);
    }
    EXPECT_EQ(file_entry_count(), 10u);

    for (int i = 0; i < 11; i++) {
        wal.append(entry);
    }
    EXPECT_EQ(file_entry_count(), 20u);

    for (int i = 0; i < 4; i++) {
        wal.append(entry);
    }
    EXPECT_EQ(file_entry_count(), 20u);

    std::ifstream file("data/wal-0.log", std::ios::binary);
    std::vector<std::byte> first_entry_bytes(entry_size);
    file.read(reinterpret_cast<char*>(first_entry_bytes.data()), static_cast<std::streamsize>(entry_size));
    EXPECT_EQ(wal.decode(first_entry_bytes), entry);

    std::remove("data/wal-0.log");
}
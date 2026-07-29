#include <gtest/gtest.h>

#include "wal.hpp"

TEST(WriteAheadLog, EncodeWorks) {
    WriteAheadLog wal;

    Log log{CommandType::PUT, "key", "value", 1000};
    
    std::vector<std::byte> bytes = {
        std::byte{0x01},
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
        std::byte{0x01},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0xE8}, std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, 
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'}, std::byte{'e'}, std::byte{'y'},
        std::byte{'v'}, std::byte{'a'}, std::byte{'l'}, std::byte{'u'}, std::byte{'e'}
    };

    EXPECT_EQ(wal.decode(bytes), log);
}
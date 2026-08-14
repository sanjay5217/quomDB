#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "wal.hpp"

class InternalKey {
    private:
    std::string user_key;
    uint64_t seq;
    CommandType type;

    public:
    InternalKey(std::string user_key, uint64_t sequence, CommandType type);

    const std::string& get_user_key() const;
    uint64_t sequence() const;
    CommandType commandType() const;

    std::vector<std::byte> encode() const;
    static InternalKey decode(const std::vector<std::byte>& bytes);

    bool operator<(const InternalKey& other) const;
    bool operator==(const InternalKey& other) const;
};

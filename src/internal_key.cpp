#include "internal_key.hpp"

#include <cstring>

namespace {
constexpr size_t TAG_SIZE = sizeof(uint64_t) + sizeof(uint8_t);
}

InternalKey::InternalKey(std::string user_key, uint64_t sequence, CommandType type)
    : user_key(std::move(user_key)), seq(sequence), type(type) {}

const std::string& InternalKey::get_user_key() const { return user_key; }

uint64_t InternalKey::get_sequence() const { return seq; }

CommandType InternalKey::get_command_type() const { return type; }

std::vector<std::byte> InternalKey::encode() const {
    std::vector<std::byte> buffer;
    buffer.reserve(user_key.size() + TAG_SIZE);

    const std::byte* key_bytes = reinterpret_cast<const std::byte*>(user_key.data());
    buffer.insert(buffer.end(), key_bytes, key_bytes + user_key.size());

    const std::byte* seq_bytes = reinterpret_cast<const std::byte*>(&seq);
    buffer.insert(buffer.end(), seq_bytes, seq_bytes + sizeof(seq));

    buffer.push_back(static_cast<std::byte>(static_cast<uint8_t>(type)));

    return buffer;
}

InternalKey InternalKey::decode(const std::vector<std::byte>& bytes) {
    size_t user_key_len = bytes.size() - TAG_SIZE;

    std::string user_key(reinterpret_cast<const char*>(bytes.data()), user_key_len);

    uint64_t sequence;
    std::memcpy(&sequence, bytes.data() + user_key_len, sizeof(sequence));

    CommandType type = static_cast<CommandType>(
        std::to_integer<uint8_t>(bytes[user_key_len + sizeof(sequence)]));

    return InternalKey(std::move(user_key), sequence, type);
}

bool InternalKey::operator<(const InternalKey& other) const {
    if (user_key != other.user_key) {
        return user_key < other.user_key;
    }
    if (seq != other.seq) {
        return seq > other.seq;
    }
    return static_cast<uint8_t>(type) < static_cast<uint8_t>(other.type);
}

bool InternalKey::operator==(const InternalKey& other) const {
    return user_key == other.user_key && seq == other.seq && type == other.type;
}

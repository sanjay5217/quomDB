#include "wal_utils.hpp"

#include <cstring>
#include <stdexcept>

uint32_t crc32(const std::vector<std::byte>& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (std::byte b : data) {
        crc ^= std::to_integer<uint32_t>(b);
        for (int i = 0; i < 8; i++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}

std::vector<std::byte> encode(Log log_entry) {
    uint32_t key_len = static_cast<uint32_t>(log_entry.key.size());
    uint32_t value_len = static_cast<uint32_t>(log_entry.value.size());

    std::vector<std::byte> payload;
    payload.reserve(sizeof(log_entry.sequence) + sizeof(CommandType) + sizeof(key_len) + sizeof(value_len) +
                     sizeof(log_entry.timestamp) + key_len + value_len);

    auto append_bytes = [&payload](const void* data, size_t size) {
        const std::byte* bytes = reinterpret_cast<const std::byte*>(data);
        payload.insert(payload.end(), bytes, bytes + size);
    };

    append_bytes(&log_entry.sequence, sizeof(log_entry.sequence));
    uint8_t command = static_cast<uint8_t>(log_entry.command);
    append_bytes(&command, sizeof(command));
    append_bytes(&key_len, sizeof(key_len));
    append_bytes(&value_len, sizeof(value_len));
    append_bytes(&log_entry.timestamp, sizeof(log_entry.timestamp));
    append_bytes(log_entry.key.data(), key_len);
    append_bytes(log_entry.value.data(), value_len);

    uint32_t checksum = crc32(payload);
    const std::byte* checksum_bytes = reinterpret_cast<const std::byte*>(&checksum);

    // final layout: [sequence][command][checksum][key_len][value_len][timestamp][key][value]
    size_t seq_and_command = sizeof(log_entry.sequence) + sizeof(CommandType);
    std::vector<std::byte> buffer;
    buffer.reserve(payload.size() + sizeof(checksum));
    buffer.insert(buffer.end(), payload.begin(), payload.begin() + seq_and_command);
    buffer.insert(buffer.end(), checksum_bytes, checksum_bytes + sizeof(checksum));
    buffer.insert(buffer.end(), payload.begin() + seq_and_command, payload.end());

    return buffer;
}

Log decode(const std::vector<std::byte>& log_entry) {
    size_t offset = 0;

    uint64_t sequence;
    std::memcpy(&sequence, log_entry.data() + offset, sizeof(sequence));
    offset += sizeof(sequence);

    CommandType command = static_cast<CommandType>(std::to_integer<uint8_t>(log_entry[offset]));
    offset += sizeof(CommandType);

    uint32_t stored_checksum;
    std::memcpy(&stored_checksum, log_entry.data() + offset, sizeof(stored_checksum));
    offset += sizeof(stored_checksum);

    uint32_t key_len;
    std::memcpy(&key_len, log_entry.data() + offset, sizeof(key_len));
    offset += sizeof(key_len);

    uint32_t value_len;
    std::memcpy(&value_len, log_entry.data() + offset, sizeof(value_len));
    offset += sizeof(value_len);

    uint64_t timestamp;
    std::memcpy(&timestamp, log_entry.data() + offset, sizeof(timestamp));
    offset += sizeof(timestamp);

    std::string key(reinterpret_cast<const char*>(log_entry.data() + offset), key_len);
    offset += key_len;

    std::string value(reinterpret_cast<const char*>(log_entry.data() + offset), value_len);
    offset += value_len;
    
    size_t seq_and_command = sizeof(sequence) + sizeof(CommandType);
    std::vector<std::byte> payload;
    payload.reserve(offset - sizeof(stored_checksum));
    payload.insert(payload.end(), log_entry.begin(), log_entry.begin() + seq_and_command);
    payload.insert(payload.end(), log_entry.begin() + seq_and_command + sizeof(stored_checksum),
                    log_entry.begin() + offset);

    if (crc32(payload) != stored_checksum) {
        throw std::runtime_error("WAL entry checksum mismatch: log entry is corrupted");
    }

    return Log{sequence, command, key, value, timestamp};
}

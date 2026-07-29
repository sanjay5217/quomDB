#include "wal.hpp"

#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>

WriteAheadLog::WriteAheadLog() {
    std::filesystem::create_directories("data");
    logfd = open("data/wal.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    bbuffer_entries = 0;
}

void WriteAheadLog::append(Log log_entry) {
    std::vector<std::byte> encoding = encode(log_entry);
    bbuffer.push_back(encoding);
    bbuffer_entries++;

    if (bbuffer_entries >= BATCH_SIZE) {
        flush();
    }

}

// do replay and flush after implementing the MemTable

void WriteAheadLog::replay() {
    // TODO
}

void WriteAheadLog::flush() {
    for (auto& entry : bbuffer) {
        write(logfd, entry.data(), entry.size());
    }
    fsync(logfd);
    bbuffer.clear();
    bbuffer_entries = 0;
}

std::vector<std::byte> WriteAheadLog::encode(Log log_entry) {
    uint32_t key_len = static_cast<uint32_t>(log_entry.key.size());
    uint32_t value_len = static_cast<uint32_t>(log_entry.value.size());

    std::vector<std::byte> buffer;
    buffer.reserve(sizeof(CommandType) + sizeof(key_len) + sizeof(value_len) +
                   sizeof(log_entry.timestamp) + key_len + value_len);

    auto append_bytes = [&buffer](const void* data, size_t size) {
        const std::byte* bytes = reinterpret_cast<const std::byte*>(data);
        buffer.insert(buffer.end(), bytes, bytes + size);
    };

    uint8_t command = static_cast<uint8_t>(log_entry.command);
    append_bytes(&command, sizeof(command));
    append_bytes(&key_len, sizeof(key_len));
    append_bytes(&value_len, sizeof(value_len));
    append_bytes(&log_entry.timestamp, sizeof(log_entry.timestamp));
    append_bytes(log_entry.key.data(), key_len);
    append_bytes(log_entry.value.data(), value_len);

    return buffer;
}

Log WriteAheadLog::decode(const std::vector<std::byte>& log_entry) const {
    size_t offset = 0;

    CommandType command = static_cast<CommandType>(std::to_integer<uint8_t>(log_entry[offset]));
    offset += sizeof(CommandType);

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

    return Log{command, key, value, timestamp};
}

WriteAheadLog::~WriteAheadLog() {
    close(logfd);
}

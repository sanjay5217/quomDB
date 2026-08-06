#include "wal.hpp"

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <unistd.h>

std::string WriteAheadLog::segmentPath(int index) {
    return "data/wal-" + std::to_string(index) + ".log";
}

void WriteAheadLog::openSegment(int index) {
    if (logfd != -1) {
        close(logfd);
    }

    segment_index = index;
    logfd = open(segmentPath(index).c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    segment_size = static_cast<size_t>(lseek(logfd, 0, SEEK_END));
}

WriteAheadLog::WriteAheadLog() {
    std::filesystem::create_directories("data");

    int latest = 0;
    for (const auto& entry : std::filesystem::directory_iterator("data")) {
        std::string filename = entry.path().filename().string();
        if (filename.rfind("wal-", 0) == 0) {
            latest = std::max(latest, std::stoi(filename.substr(4)));
        }
    }

    openSegment(latest);
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
        segment_size += entry.size();
    }
    fsync(logfd);
    bbuffer.clear();
    bbuffer_entries = 0;

    if (segment_size >= SEGMENT_SIZE_LIMIT) {
        openSegment(segment_index + 1);
    }
}

uint32_t WriteAheadLog::crc32(const std::vector<std::byte>& data) {
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

std::vector<std::byte> WriteAheadLog::encode(Log log_entry) {
    uint32_t key_len = static_cast<uint32_t>(log_entry.key.size());
    uint32_t value_len = static_cast<uint32_t>(log_entry.value.size());

    std::vector<std::byte> payload;
    payload.reserve(sizeof(CommandType) + sizeof(key_len) + sizeof(value_len) +
                     sizeof(log_entry.timestamp) + key_len + value_len);

    auto append_bytes = [&payload](const void* data, size_t size) {
        const std::byte* bytes = reinterpret_cast<const std::byte*>(data);
        payload.insert(payload.end(), bytes, bytes + size);
    };

    uint8_t command = static_cast<uint8_t>(log_entry.command);
    append_bytes(&command, sizeof(command));
    append_bytes(&key_len, sizeof(key_len));
    append_bytes(&value_len, sizeof(value_len));
    append_bytes(&log_entry.timestamp, sizeof(log_entry.timestamp));
    append_bytes(log_entry.key.data(), key_len);
    append_bytes(log_entry.value.data(), value_len);

    uint32_t checksum = crc32(payload);
    const std::byte* checksum_bytes = reinterpret_cast<const std::byte*>(&checksum);

    std::vector<std::byte> buffer;
    buffer.reserve(payload.size() + sizeof(checksum));
    buffer.insert(buffer.end(), payload.begin(), payload.begin() + sizeof(CommandType));
    buffer.insert(buffer.end(), checksum_bytes, checksum_bytes + sizeof(checksum));
    buffer.insert(buffer.end(), payload.begin() + sizeof(CommandType), payload.end());

    return buffer;
}

Log WriteAheadLog::decode(const std::vector<std::byte>& log_entry) const {
    size_t offset = 0;

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

    std::vector<std::byte> payload;
    payload.reserve(offset - sizeof(stored_checksum));
    payload.insert(payload.end(), log_entry.begin(), log_entry.begin() + sizeof(CommandType));
    payload.insert(payload.end(), log_entry.begin() + sizeof(CommandType) + sizeof(stored_checksum),
                    log_entry.begin() + offset);

    if (crc32(payload) != stored_checksum) {
        throw std::runtime_error("WAL entry checksum mismatch: log entry is corrupted");
    }

    return Log{command, key, value, timestamp};
}

WriteAheadLog::~WriteAheadLog() {
    close(logfd);
}

#include "wal.hpp"
#include "wal_utils.hpp"

#include <fcntl.h>
#include <filesystem>
#include <unistd.h>

WriteAheadLog::WriteAheadLog() {
    std::filesystem::create_directories("data");
    logfd = open("data/wal.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
}

void WriteAheadLog::append(Log log_entry) {
    std::vector<std::byte> encoding = encode(log_entry);
    write(logfd, encoding.data(), encoding.size());
    fsync(logfd);
}

void WriteAheadLog::replay() {
    // TODO
}

WriteAheadLog::~WriteAheadLog() {
    close(logfd);
}

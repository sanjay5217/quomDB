#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "wal.hpp"

uint32_t crc32(const std::vector<std::byte>& data);
std::vector<std::byte> encode(Log log_entry);
Log decode(const std::vector<std::byte>& log_entry);

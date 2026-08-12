cmake_minimum_required(VERSION 3.16)
project(quom CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(quom_lib
    src/storage.cpp
    src/wal.cpp
    src/wal_utils.cpp
)
target_include_directories(quom_lib PUBLIC include)

add_executable(quom src/main.cpp)
target_link_libraries(quom PRIVATE quom_lib)

enable_testing()
add_subdirectory(test)

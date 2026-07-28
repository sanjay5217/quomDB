#pragma once

#include <string>
#include <vector> 

#include "storage.hpp"

enum class Type {GET, PUT, DELETE};

struct Command {
    Type type;
    Key key;
    Value value;
};

class WriteAheadLog {
    public:
    void append(Command command);
    void replay(void);
    void flush(void);
    int encode(Command command);
    Command decode(int log_entry);

    private:
    int log_fd;
};
#include <iostream>
#include <sstream>
#include <string>

#include "storage.hpp"

int main() {
    StorageEngine engine;
    std::string line;

    std::cout << "quom> ";
    while (std::getline(std::cin, line)) {
        std::istringstream stream(line);
        std::string command, key, value;
        stream >> command >> key;
        std::getline(stream, value);
        if (!value.empty() && value.front() == ' ') {
            value.erase(0, 1);
        }

        if (command == "help" && key.empty() && value.empty()) {
            std::cout << 
                "quomdb cli\n"
                "commands:\n"
                "---------------------------------------------------------\n"
                "put <key> <value>       Insert or update a key-value pair\n"
                "get <key>               Retrieve a value\n"
                "delete <key>            Delete a key-value pair\n"
                "help                    Display available commands\n"
                "exit                    Close the database connection\n"
                "---------------------------------------------------------\n";
        } else if (command == "put") {
            if (key.empty() || value.empty()) {
                std::cout << "usage: put <key> <value>\n";
            } else {
                engine.put(key, value);
            }
        } else if (command == "get") {
            if (key.empty()) {
                std::cout << "usage: get <key>\n";
            } else {
                auto result = engine.get(key);
                std::cout << (result.has_value() ? *result : "null") << "\n";
            }
        } else if (command == "delete") {
            if (key.empty()) {
                std::cout << "usage: delete <key>\n";
            } else {
                std::cout << (engine.del(key) ? "" : "null\n");
            }
        } else if (command == "exit") {
            break;
        } else if (!command.empty()) {
            std::cout << "unknown command: " << command << " (expected put/get/delete/help/exit)\n";
        }

        std::cout << "quom> ";
    }

    std::cout << "\n";
    return 0;
}

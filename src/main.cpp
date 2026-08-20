#include <iostream>
#include <string>
#include "db.h"

void printHelp()
{
    std::cout << "\n=== MiniDB Commands ===\n";
    std::cout << "  set <key> <value>  - Store a key-value pair\n";
    std::cout << "  get <key>          - Retrieve a value\n";
    std::cout << "  remove <key>       - Delete a key\n";
    std::cout << "  clear              - Clear Terminal\n";
    std::cout << "  help               - Show this message\n";
    std::cout << "  exit               - Quit MiniDB\n";
    std::cout << "=======================\n\n";
}

int main()
{
    std::cout << "=== MiniDB v0.1 ===\n";
    std::cout << "Type 'help' for commands\n\n";

    // open database
    MiniDB db("./data");

    std::string current_command;

    while (true)
    {
        std::cout << "minidb> ";
        std::cin >> current_command;

        if (current_command == "set")
        {
            std::string key, value;
            std::cin >> key >> value;
            db.put(key, value);
            std::cout << "OK\n";
        }
        else if (current_command == "get")
        {
            std::string key;
            std::cin >> key;

            auto value = db.get(key);
            if (value.has_value())
            {
                std::cout << value.value() << "\n";
            }
            else
            {
                std::cout << "(nil)\n";
            }
        }
        else if (current_command == "remove")
        {
            std::string key;
            std::cin >> key;
            db.remove(key);
            std::cout << "OK\n";
        }
        else if (current_command == "help")
        {
            printHelp();
        }
        else if (current_command == "exit")
        {
            std::cout << "Goodbye!\n";
            break;
        }
        else if (current_command == "clear")
        {
            std::cout << "\033[2J\033[1;1H" << std::flush; // flushes the output buffer
        }
        else
        {
            std::cout << "Unknown command: " << current_command << "\n";
            std::cout << "Type 'help' for available commands\n";
        }
    }
}
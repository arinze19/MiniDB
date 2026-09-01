#include <iostream>
#include <string>
#include "db.h"

void printHelp(MiniDB::IndexType type)
{
    std::cout << "\n=== MiniDB Commands ===\n";
    std::cout << "  set <key> <value>  - Store a key-value pair\n";
    std::cout << "  get <key>          - Retrieve a value\n";
    std::cout << "  del <key>          - Delete a key\n";
    std::cout << "  keys               - Retrieve all keys\n";
    std::cout << "  compact            - Compact (Merge + Clean) segments\n";
    std::cout << "  flush              - Flush memtable to disk\n";
    std::cout << "  stats              - Show DB statistics\n";
    std::cout << "  clear              - Clear Terminal\n";
    if (type == MiniDB::IndexType::BTREE)
    {
        std::cout << " range <start> <end> - Get all keys in a certain range (inclusive of start and end) \n";
    }
    std::cout << "  help               - Show this message\n";
    std::cout << "  exit               - Quit MiniDB\n";
    std::cout << "=======================\n\n";
}

// terminal flags | Didn't know about that
int main(int argc, char *argv[])
{
    std::cout << "=== MiniDB v0.2 ===\n";
    std::cout << "Type 'help' for commands\n\n";

    // Parse --index flag from CLI
    // Usage: ./minidb --index=hash  or  ./minidb --index=btree
    MiniDB::IndexType indexType = MiniDB::IndexType::HASH; // Default to hash

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "--index=btree")
        {
            indexType = MiniDB::IndexType::BTREE;
            std::cout << "[MiniDB] Using BTree index\n";
        }
        else if (arg == "--index=hash")
        {
            indexType = MiniDB::IndexType::HASH;
            std::cout << "[MiniDB] Using Hash index\n";
        }
    }

    // open database
    MiniDB db("./data", indexType);

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
        else if (current_command == "del")
        {
            std::string key;
            std::cin >> key;
            db.remove(key);
            std::cout << "OK\n";
        }
        else if (current_command == "flush")
        {
            std::cout << "Flushing memtable..."
                      << std::endl;
            db.flushMemtable();
            std::cout
                << "Done!" << std::endl;
        }
        else if (current_command == "range")
        {
            std::string start, end; // similar to python first, second = 0, 0?
            std::cin >> start >> end;

            try
            {
                auto results = db.range(start, end);
                if (results.empty())
                {
                    std::cout << "(no results)\n";
                }
                else
                {
                    for (const auto &[key, value] : results)
                    {
                        std::cout << key << "->" << value << "\n";
                    }
                }
            }
            catch (const std::exception &e)
            {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
        else if (current_command == "compact")
        {
            std::cout << "[MiniDB]: Starting compaction process..." << std::endl;
            db.compact();
            std::cout << "[MiniDB]: Finished compaction process, segments count now " << db.getSegmentCount() << std::endl;
        }
        else if (current_command == "stats")
        {
            std::cout << "Keys: " << db.indexSize() << "\n";
            std::cout << "Segments: " << db.getSegmentCount() << "\n";
            std::cout << "Index Type: " << (indexType == MiniDB::IndexType::BTREE ? "BTree" : "Hash") << " index" << "\n";
        }
        else if (current_command == "help")
        {
            printHelp(indexType);
        }
        else if (current_command == "keys")
        {
            auto all_keys = db.keys(); // get all keys

            if (all_keys.empty())
            {
                std::cout << "(Empty) \n";
            }
            else
            {
                for (size_t i = 0; i < all_keys.size(); i++)
                {
                    std::cout << i + 1 << ") " << all_keys[i] << "\n";
                }
            }
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
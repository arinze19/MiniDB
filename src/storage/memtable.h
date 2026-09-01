#pragma once

#include <map>
#include <string>
#include <optional>
#include <vector>
#include "segment.h"

// write into memtable then periodically write into segment
static constexpr size_t DEAULT_MAX_MEMTABLE_SIZE = 4 * 1024 * 1024;

// what's your emotions like today

class Memtable
{
public:
    explicit Memtable(size_t max_size = DEFAULT_MAX_SEGMENT_SIZE);

    // valiate if key is in memtable
    // check disk next if not in memtable
    std::optional<std::string> get(std::string &key) const;

    void put(const std::string &key, const std::string &value);

    std::optional<std::string> get(const std::string &key);

    void remove(const std::string &key);

    bool contains(const std::string &key) const;

    bool isFull() const;

    bool isEmpty() const;

    // TODO: why bother return when flush?
    std::vector<Record> flush();

    // Current memory usage in bytes
    size_t sizeBytes() const;

    size_t count() const; // number of entries of entries (tombstone inclusive)

    void clear(); // after successful flushed to disk

private:
    // why struct
    struct Entry
    {
        std::string value;
        bool tombstone = false;
    };

    std::map<std::string, Entry> table; // why need to be sorted

    size_t max_size;
    size_t current_size;

    // how much memory is used by one insert
    size_t entrySize(const std::string &key, const std::string &value) const;
};

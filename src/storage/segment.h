#pragma once // research

#include <string>
#include <fstream>
#include <optional>
#include <cstdint>

// [key_size(4 bytes)][value_size(4 bytes)][tombstone(1 byte)][key][value]
struct Record
{
    std::string key;
    std::string value;
    bool tombstone = false; // default tombstone to false
};

class Segment
{
public:
    explicit Segment(const std::string &path); // explicit forces construction call

    ~Segment(); 

    size_t write(const Record &record);

    std::optional<Record> read(size_t offset);

    std::vector<Record> readAll();

    size_t size() const;

    const std::string &path() const;

private:
    std::fstream file;
    std::string filepath;
    size_t filesize;

    // convert to bytes and write directly into the disk
    void writeUint32(uint32_t value);

    // read bytes from disk
    uint32_t readUint32();
};
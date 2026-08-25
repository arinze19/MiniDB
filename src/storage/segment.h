#pragma once // research

#include <string>
#include <fstream>
#include <optional>
#include <cstdint>

// default segment size 
// compile time constant - stand alone constant which is shared
static constexpr size_t DEFAULT_MAX_SEGMENT_SIZE = 1024 * 1024;

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
    explicit Segment(const std::string &path); // explicit keyword forces construction call

    ~Segment(); 

    size_t write(const Record &record);

    std::optional<Record> read(size_t offset);

    std::vector<Record> readAll();

    size_t size() const;

    const std::string &path() const;

    // validate if segment has exceeded maximum limit
    // TODO: remove this. allowing for passing max_size args for testing purposes mainly
    bool isFull(size_t max_size = DEFAULT_MAX_SEGMENT_SIZE) const;

    void deleteFile(); 

private:
    std::fstream file;
    std::string filepath;
    size_t filesize;

    // convert to bytes and write directly into the disk
    void writeUint32(uint32_t value);

    // read bytes from disk
    uint32_t readUint32();
};
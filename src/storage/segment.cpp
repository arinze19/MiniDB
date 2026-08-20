#include "segment.h"
#include <iostream>
#include <stdexcept> // try-catch?

// Constructor
Segment::Segment(const std::string &path) : filepath(path), filesize(0) // member initializer list
{

    file.open(filepath, std::ios::in | std::ios::out | std::ios::binary | std::ios::app); // why do we use ios::in and ios::out

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file segment: " + filepath); // is this enabled by <stdexcept>
    };

    file.seekg(0, std::ios::end); // move read pointer to end of file
    filesize = file.tellg();      // do we need to specify as
}

// Destructor
Segment::~Segment()
{
    if (file.is_open())
    {
        file.close();
    }
}

// Write to disk
size_t Segment::write(const Record &record)
{
    size_t offset = filesize;

    // specify where file should start writing from
    file.seekp(0, std::ios::end);

    // 4 bytes
    writeUint32(static_cast<uint32_t>(record.key.size())); // write the key size

    // 4 bytes
    writeUint32(static_cast<uint32_t>(record.value.size())); // write the value size

    // 1 bytes
    char tombstone = record.tombstone ? 1 : 0; // storing byte values directly

    // Struggling to understand this
    file.write(&tombstone, 1); // why are we writing the memory address

    file.write(record.key.data(), record.key.size()); // write key to file

    file.write(record.value.data(), record.value.size()); // write data to file

    file.flush(); // For durability?

    filesize += 4 + 4 + 1 + record.value.size() + record.value.size(); // why do we use hardcoded values of 4, 4 and 1

    return offset; // returns where the particular record starts?
};

std::optional<Record> Segment::read(size_t offset)
{
    // jump to offset of file
    file.seekg(offset);

    if (file.fail())
    {
        return std::nullopt; // offset out of bound
    }

    uint32_t key_size = readUint32(); // read key size

    uint32_t value_size = readUint32(); // read value size

    char tombstone_byte;           // why don't we call this variable "tombstone just like we have in the write function"?
    file.read(&tombstone_byte, 1); // what does this do?

    // Read key
    std::string key(key_size, '\0'); // create a key string (constructor) filled with null bytes and of size key_size
    file.read(key.data(), key.size());

    // Read value
    std::string value(value_size, '\0'); // create a value string (constructor) filled with null bytes and of size value_size
    file.read(value.data(), value.size());

    if (file.fail())
    {
        return std::nullopt;
    }

    return Record{key, value, tombstone_byte == 1}; // positional mapping
}

std::vector<Record> Segment::readAll()
{
    std::vector<Record> records;

    file.clear(); // clears previously used flags on the file stream
    file.seekg(0, std::ios::beg);

    size_t pos = 0;

    while (pos < filesize)
    {
        auto record = read(pos);

        if (!record.has_value())
            break;

        // advance position
        // why do we use arrow here? I think we use the arrow because record is essentially a
        pos += 4 + 4 + 1 + record->key.size() + record->value.size();
        records.push_back(*record); // dereferencing to get actual values into the application
    }

    return records;
}

void Segment::writeUint32(uint32_t value)
{
    // TOOO: Inspect
    char bytes[4] = {
        static_cast<char>((value >> 24) & 0xFF), // shift all bits to the right (24 spaces) and and keep last 8 bits
        static_cast<char>((value >> 16) & 0xFF), // shift all bits to the right (16 spaces) and and keep last 8 bits
        static_cast<char>((value >> 8) & 0xFF),  // shift all bits to the right (8 spaces) and and keep last 8 bits
        static_cast<char>((value) & 0xFF)};

    file.write(bytes, 4);
}

uint32_t Segment::readUint32()
{
    // TOOO: Inspect
    char bytes[4];       // create bytes | null bytes
    file.read(bytes, 4); // read 4 bytes from where ever the read cursor is?

    return (static_cast<uint32_t>(static_cast<unsigned char>(bytes[0])) << 24) |
           (static_cast<uint32_t>(static_cast<unsigned char>(bytes[1])) << 16) |
           (static_cast<uint32_t>(static_cast<unsigned char>(bytes[2])) << 8) |
           (static_cast<uint32_t>(static_cast<unsigned char>(bytes[3])));
}

// Get file size
size_t Segment::size() const
{
    return filesize;
}

// Get file path
const std::string &Segment::path() const
{
    return filepath;
}

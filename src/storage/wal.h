#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include "segment.h"

// WAL -> memtable -> disk -> index
// [op_type: 1 byte][key_size: 4 bytes][val_size: 4 bytes][key][value]

// OP_TYPES =
class WAL
{
public:
    static constexpr uint8_t PUT = 0x01; // using hex codes for readability since byte oriented numbers
    static constexpr uint8_t DELETE = 0x02;

    explicit WAL(const std::string &path);
    ~WAL();

    void logPut(const std::string &key, const std::string &value);

    void logDelete(const std::string &key);

    std::vector<Record> replay();

    void clear();

    bool isEmpty() const;

    size_t size() const;

private:
    std::string file_path;
    std::ofstream writer;
    size_t file_size;

    void writeUint8(uint8_t value);
    void writeUint32(uint32_t value);

    static uint8_t readUint8(std::ifstream &f);
    static uint32_t readUint32(std::ifstream &f);
};
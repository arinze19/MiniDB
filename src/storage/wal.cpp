#include "wal.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

WAL::WAL(const std::string &path) : file_path(path), file_size(0)
{
    writer.open(path, std::ios::binary | std::ios::app);

    if (!writer.is_open())
    {
        throw std::runtime_error("Failed to open WAL file: " + path);
    }

    file_size = std::filesystem::file_size(path);

    if (file_size > 0)
    {
        std::cout << "[WAL] Found existing WAL (" << file_size << ") ~ crash recovery may be needed" << std::endl;
    }
};

WAL::~WAL()
{
    if (writer.is_open())
    {
        writer.flush(); // write memory buffer into OS buffer?
        writer.close();
    }
}

void WAL::logPut(const std::string &key, const std::string &value)
{
    writeUint8(PUT);
    writeUint32(static_cast<uint32_t>(key.size()));
    writeUint32(static_cast<uint32_t>(value.size()));
    writer.write(key.data(), key.size());
    writer.write(value.data(), value.size());

    writer.flush();

    file_size += 1 + 4 + 4 + key.size() + value.size();
}

void WAL::logDelete(const std::string &key)
{
    writeUint8(DELETE);
    writeUint32(static_cast<uint32_t>(key.size()));
    writeUint32(0);
    writer.write(key.data(), key.size());

    writer.flush();

    file_size += 1 + 4 + 4 + key.size();
}

void WAL::writeUint8(uint8_t value)
{
    writer.write(reinterpret_cast<const char *>(&value), 1); // treat as pointer to raw bytes | uint8_t* with address 
}

void WAL::writeUint32(uint32_t value)
{
    char bytes[4] = {
        static_cast<char>((value >> 24) & 0xFF),
        static_cast<char>((value >> 16) & 0xFF),
        static_cast<char>((value >> 8) & 0xFF),
        static_cast<char>((value) & 0xFF),
    };

    writer.write(bytes, 4);
}

uint8_t readUint8(std::ifstream &f)
{
    uint8_t value;
    f.read(reinterpret_cast<char *>(&value), 1);

    if (f.fail())
    {
        throw std::runtime_error("WAL read error");
    }

    return value;
}

uint32_t readUint32(std::ifstream &f)
{
    char bytes[4];

    f.read(bytes, 4);

    if (f.fail())
    {
        throw std::runtime_error("WAL read error");
    }

    return (
        static_cast<uint32_t>(static_cast<unsigned char>(bytes[0]) << 24) |
        static_cast<uint32_t>(static_cast<unsigned char>(bytes[1]) << 16) |
        static_cast<uint32_t>(static_cast<unsigned char>(bytes[2]) << 8) |
        static_cast<uint32_t>(static_cast<unsigned char>(bytes[3])));
}

void WAL::clear()
{
    writer.close(); // why close first

    std::ofstream truncate(file_path, std::ios::binary | std::ios::trunc);
    truncate.close();

    // reopen for append
    writer.open(file_path, std::ios::binary | std::ios::app);

    if (!writer.is_open())
    {
        throw std::runtime_error("Failed to reopen WAL after clear");
    }

    file_size = 0;
    std::cout << "[WAL] Cleared - data safely persisted to disk" << std::endl;
}

bool WAL::isEmpty() const
{
    return file_size == 0;
}

size_t WAL::size() const
{
    return file_size;
}
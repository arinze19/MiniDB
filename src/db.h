#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include "storage/segment.h"

class MiniDB
{
public:
    explicit MiniDB(const std::string &data_dir);

    /**
     * If your class is meant to be inherited from,
     * you must define a virtual destructor so derived objects
     * are destroyed properly through base-class pointers:
     */
    ~MiniDB() = default;

    void put(const std::string &key, const std::string &value); // const here means the function promises not to change the value right?

    std::optional<std::string> get(const std::string &key);

    bool remove(const std::string &key);

private:
    std::string data_dir;
    std::unique_ptr<Segment> segment; // what does unique_ptr do?

    std::unordered_map<std::string, size_t> hash_index; // this is basically like a python list right?

    void buildHashIndex(); // build index once segment starts up
};
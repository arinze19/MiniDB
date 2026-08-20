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

    void put(const std::string &key, const std::string &value);

    std::optional<std::string> get(const std::string &key);

    bool remove(const std::string &key);

private:
    std::string data_dir;
    // with following setup we can opt to
    // create segment at another part in our program initializer list
    // taking advantage of unique_ptr as well so we can dynamically create/delete segments when compacting
    std::unique_ptr<Segment> segment;

    std::unordered_map<std::string, size_t> hash_index;

    void buildHashIndex(); // build index once segment starts up
};
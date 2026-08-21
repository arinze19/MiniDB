#pragma once

#include <memory>
#include <optional>
#include <string>
#include "storage/segment.h"
#include "index/hash_index.h"

class MiniDB
{
public:
    // Determine index type to use for DB
    enum class IndexType
    {
        HASH,
        BTREE
    };

    // create DB with chosen index
    // defaulting to hash indexing
    explicit MiniDB(const std::string &data_dir, IndexType index = IndexType::HASH);

    /**
     * If your class is meant to be inherited from,
     * you must define a virtual destructor so derived objects
     * are destroyed properly through base-class pointers:
     */
    ~MiniDB() = default;

    void put(const std::string &key, const std::string &value);

    std::optional<std::string> get(const std::string &key);

    bool remove(const std::string &key);

    // Index information
    size_t indexSize() const;

    std::vector<std::string> keys() const;

private:
    std::string data_dir;
    // with following setup we can opt to
    // create segment at another part in our program initializer list
    // taking advantage of unique_ptr as well so we can dynamically create/delete segments when compacting
    std::unique_ptr<Segment> segment;
    std::unique_ptr<Index> index;

    void buildIndex();

    // Factory method - creates the right index type
    // what is "static" - only MiniDB can call this method
    static std::unique_ptr<Index> createIndex(IndexType type);
};
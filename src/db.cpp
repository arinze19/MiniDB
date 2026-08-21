#include "db.h"
#include <filesystem>
#include <iostream>

// Factory: Creates the right index type
// What is a factory?
std::unique_ptr<Index> MiniDB::createIndex(IndexType type)
{
    switch (type)
    {
    case IndexType::HASH:
        return std::make_unique<HashIndex>();
    case IndexType::BTREE:
        throw std::runtime_error("BTree Indexing not supported yet!");
    default:
        std::cout << "Index type is unknown - Defaulting to HashIndex" << std::endl;
        return std::make_unique<HashIndex>();
    }
}

MiniDB::MiniDB(const std::string &dir, IndexType type) : data_dir(dir)
{
    if (!std::filesystem::exists(data_dir))
    {
        std::filesystem::create_directories(data_dir);
    }

    index = createIndex(type); // create index type

    std::string segment_path = data_dir + "/data.seg";
    // create a segment using the segment path
    // is .seg a recognized file format or could it have been end extension? -> for all we know the segment.seg file represents the class from segment.cpp
    segment = std::make_unique<Segment>(segment_path);

    // Build index
    buildIndex();

    std::cout << "Database loaded at " << data_dir << "\n";
    std::cout << "Index loaded with " << index->size() << "\n";
}

void MiniDB::put(const std::string &key, const std::string &value)
{
    Record record = Record{key, value, false}; // is this how we define objects

    size_t offset = segment->write(record);

    index->put(key, offset); // setting index keys
}

std::optional<std::string> MiniDB::get(const std::string &key)
{
    // check if key exists in index
    // returns an iterator
    // using auto here so iterator type is automatically inferred as opposed to writing out the whole type of the map
    auto offset = index->get(key);

    // maps must return a value regardless
    // so in the instance where key is not found
    // it returns an iterator pointing to the sentinel just after the end of the map
    // also if an item is not in the index it is most likely tombstoned
    if (!offset.has_value())
    {
        return std::nullopt;
    };

    // using auto since it returns optional
    auto record = segment->read(*offset); // why do we have to use *offset for this?

    // adding !record.has_value() here to
    // prevent invalid state crashes in the system
    if (record->tombstone || !record.has_value())
    {
        return std::nullopt;
    }

    return record->value;
}

bool MiniDB::remove(const std::string &key)
{
    // null case
    if (!index->contains(key))
    {
        std::cout << "[MiniDB]: Key not found" << "\n";
        return false;
    }

    Record tombstone{key, "", true};

    // add to db
    size_t offset = segment->write(tombstone);

    // update index
    index->put(key, offset);

    return true;
}

std::vector<std::string> MiniDB::keys() const
{
    // get the keys in the index
    return index->keys();
}

void MiniDB::buildIndex()
{
    // get all records from the segment? what if there are multiple segments
    auto records = segment->readAll();
    size_t offset = 0;

    for (const auto &record : records)
    {
        if (record.tombstone)
        {
            index->remove(record.key); // what does erase do?
        }
        else
        {
            index->put(record.key, offset);
        }

        // update offset
        offset += 4 + 4 + 1 + record.key.size() + record.value.size();
    }
}
